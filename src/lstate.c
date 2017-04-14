/*
 * Fluto - the web server.
 *
 * Copyright (c) 2016-2017, Dmitry Kobylin
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <string.h>
#include <unistd.h>
/* */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
/* */
#include "debug.h"
#include "http.h"
#include "lscript/init0.h"
#include "lscript/init1.h"
#include "lscript/process.h"
#include "lstate.h"

#if 0
    #define DEBUG_THIS
#endif

static int _debugPrint(lua_State *L);
static int _getContent(lua_State *L);
static int _writeSock(lua_State *L);

/*
 * Initialize lua state on client creation.
 */
int lstateInit0(struct client_t *client)
{
    lua_State *L = client->luaState;

    lua_pushcfunction(L, _debugPrint); lua_setglobal(L, "debugPrint");

    if (luaL_loadbufferx(L, init0Script, sizeof(init0Script), "init0Script", "b") != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to load init0 script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to execute init0 script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }     

    lua_pushlightuserdata(L, client); lua_setglobal(L, "client");

    lua_pushinteger(L, HTTP_ERROR_200_OK                 ); lua_setglobal(L, "HTTP_ERROR_200_OK");
    lua_pushinteger(L, HTTP_ERROR_204_NO_CONTENT         ); lua_setglobal(L, "HTTP_ERROR_204_NO_CONTENT");
    lua_pushinteger(L, HTTP_ERROR_400_BAD_REQUEST        ); lua_setglobal(L, "HTTP_ERROR_400_BAD_REQUEST");
    lua_pushinteger(L, HTTP_ERROR_403_FORBIDDEN          ); lua_setglobal(L, "HTTP_ERROR_403_FORBIDDEN");
    lua_pushinteger(L, HTTP_ERROR_404_NOT_FOUND          ); lua_setglobal(L, "HTTP_ERROR_404_NOT_FOUND");
    lua_pushinteger(L, HTTP_ERROR_405_METHOD_NOT_ALLOWED ); lua_setglobal(L, "HTTP_ERROR_405_METHOD_NOT_ALLOWED");
    lua_pushinteger(L, HTTP_ERROR_409_CONFLICT           ); lua_setglobal(L, "HTTP_ERROR_409_CONFLICT");

    lua_pushinteger(L, DLEVEL_SILENT ); lua_setglobal(L, "DLEVEL_SILENT");
    lua_pushinteger(L, DLEVEL_ERROR  ); lua_setglobal(L, "DLEVEL_ERROR");
    lua_pushinteger(L, DLEVEL_WARNING); lua_setglobal(L, "DLEVEL_WARNING");
    lua_pushinteger(L, DLEVEL_INFO   ); lua_setglobal(L, "DLEVEL_INFO");
    lua_pushinteger(L, DLEVEL_NOISE  ); lua_setglobal(L, "DLEVEL_NOISE");

    lua_getglobal(L, "request");     /* [request]->TOS */
    lua_pushcfunction(L, _getContent); /* [request][getContent]->TOS */
    lua_setfield(L, -2, "getContent"); /* [request]->TOS */
    lua_pop(L, 1);                    /* ->TOS */

    lua_getglobal(L, "response");     /* [response]->TOS */
    lua_pushcfunction(L, _writeSock); /* [response][writeSock]->TOS */
    lua_setfield(L, -2, "writeSock"); /* [response]->TOS */
    lua_pop(L, 1);                    /* ->TOS */

#if (1 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
    return 0;
}
/*
 * Initialize lua state on new request.
 */
int lstateInit1(struct client_t *client)
{
    lua_State *L = client->luaState;

    if (luaL_loadbufferx(L, init1Script, sizeof(init1Script), "init1Script", "b") != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to load init1 script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to execute init1 script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }     

#if (1 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
    return 0;
}
/*
 * Process http request.
 *
 * RETURN
 *     0 if connection must be dropped, 1 otherwise.
 */
int lstateProcessRequest(struct client_t *client, int httpError)
{
    lua_State *L = client->luaState;

    lua_pushinteger(L, httpError);
    lua_setglobal(L, "httpError");

    if (luaL_loadbufferx(L, processScript, sizeof(processScript), "processScript", "b") != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to load process script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return 0;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        DEBUG_CLIENT(DLEVEL_NOISE, "Failed to execute process script: \"%s\".",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return 0;
    }     

    return 1;
}
/*
 *
 */
static int _debugPrint(lua_State *L)
{
    struct client_t *client;
    int argc;
    int isnum;
    int level;
    const char *s;
#define _LSTATE_DEBUG_PRINT_LEVEL_ARG    1
#define _LSTATE_DEBUG_PRINT_STRING_ARG   2
    argc = lua_gettop(L);
    if (argc < _LSTATE_DEBUG_PRINT_STRING_ARG)
        luaL_error(L, "not enough arguments");

    level = lua_tointegerx(L, _LSTATE_DEBUG_PRINT_LEVEL_ARG, &isnum);
    if (!isnum)
        luaL_argerror(L, _LSTATE_DEBUG_PRINT_LEVEL_ARG, "not a number");
    s = lua_tostring(L, _LSTATE_DEBUG_PRINT_STRING_ARG);
    if (!s)
        luaL_argerror(L, _LSTATE_DEBUG_PRINT_STRING_ARG, "not a string");

    lua_getglobal(L, "client");
    client = lua_touserdata(L, -1);
    lua_pop(L, 1);

#if 0
    debugPrint(level, (char*)s);
#else
    DEBUG_CLIENT(level, "%s", s);
#endif
    return 0;
}
/*
 *
 * RETURN
 *     On stack index -1 string with content. Empty string if no content remain / error.
 */
static int _getContent(lua_State *L)
{
    struct client_t *client;
    int len;
    int r;
    int n;
    int isnum;
    int argc;
    luaL_Buffer lbuf;
    char *data;
#define _LSTATE_GET_CONTENT_LEN_ARG    1

    argc = lua_gettop(L);
    if (argc < _LSTATE_GET_CONTENT_LEN_ARG)
        luaL_error(L, "not enough arguments");

    len = lua_tointegerx(L, _LSTATE_GET_CONTENT_LEN_ARG, &isnum);
    if (!isnum)
        luaL_argerror(L, _LSTATE_GET_CONTENT_LEN_ARG, "not a number");
    if (len < 0)
        luaL_argerror(L, _LSTATE_GET_CONTENT_LEN_ARG, "must be positive");

    lua_getglobal(L, "client");
    client = lua_touserdata(L, -1);
    lua_pop(L, 1);

    data = luaL_buffinitsize(L, &lbuf, len);
    r = tokenGetRemainedData(&client->token, data, len);
    n = r;
    if (n < len)
    {
        data += n;
        r = (*client->getChars)(data, len - n, client);
        if (r > 0)
            n += r;
    }
    luaL_pushresultsize(&lbuf, n);

    return 1;
}
/*
 *
 */
static int _writeSock(lua_State *L)
{
    const char *data;
    struct client_t *client;
    size_t len;

    int argc;
#define _WRITE_SOCK_DATA_ARG       1
    argc = lua_gettop(L);
    if (argc < _WRITE_SOCK_DATA_ARG)
        luaL_error(L, "no data to write specified");
    data = lua_tolstring(L, _WRITE_SOCK_DATA_ARG, &len);
    if (!data)
        luaL_argerror(L, _WRITE_SOCK_DATA_ARG, "must be string");

    lua_getglobal(L, "client");
    client = lua_touserdata(L, -1);
    lua_pop(L, 1);

    /* TODO raise error instead of boolean */
    if (write(client->sock, data, len) < 0)
        luaL_error(L, "write to socket failed");

    return 0;
}
/*
 *
 */
static void _setRequestField(lua_State *L, char *field, char *value, int *vlen)
{
    lua_getglobal(L, "request");      /* [request]->TOS */
    if (vlen)
        lua_pushlstring(L, value, *vlen);  /* [request][value]->TOS */
    else
        lua_pushstring(L, value);  /* [request][value]->TOS */
    lua_setfield(L, -2, field);       /* [request]->TOS */
    lua_pop(L, 1);                    /* ->TOS */
#if (0 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
}
void lstateSetRequestField(lua_State *L, char *field, char *value)
{
    _setRequestField(L, field, value, NULL);
}
void lstateSetRequestFieldL(lua_State *L, char *field, char *value, int vlen)
{
    _setRequestField(L, field, value, &vlen);
}
/*
 *
 */
void _appendRequestField(lua_State *L, char *field, char *value, int *vlen)
{
    lua_getglobal(L, "request");      /* [request]->TOS */
    lua_getfield(L, -1, field);       /* [request][oldval]->TOS */
    if (vlen)
        lua_pushlstring(L, value, *vlen);  /* [request][oldval][newval]->TOS */
    else
        lua_pushstring(L, value);  /* [request][oldval][newval]->TOS */
    lua_concat(L, 2);                 /* [request][oldval..newval]->TOS */
    lua_setfield(L, -2, field);       /* [request]->TOS */
    lua_pop(L, 1);                    /* ->TOS */
#if (0 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). NOTE Not empty on parsing query string. */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
}
void lstateAppendRequestField(lua_State *L, char *field, char *value)
{
    _appendRequestField(L, field, value, NULL);
}
void lstateAppendRequestFieldL(lua_State *L, char *field, char *value, int vlen)
{
    _appendRequestField(L, field, value, &vlen);
}
/*
 *
 */
void lstateSetRequestFieldNum(lua_State *L, char *field, int64_t num)
{
    lua_getglobal(L, "request");           /* [request]->TOS */
    lua_pushinteger(L, (lua_Integer)num);  /* [request][num]->TOS */
    lua_setfield(L, -2, field);            /* [request]->TOS */
    lua_pop(L, 1);                         /* ->TOS */
#if (0 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
}
/*
 * Set temporary value in client's lua state.
 *
 */
static void _setGlobalVal(lua_State *L, char *name, char *value, int *vlen)
{
    if (vlen)
        lua_pushlstring(L, value, *vlen);  /* [value]->TOS */
    else
        lua_pushstring(L, value);  /* [value]->TOS */
    lua_setglobal(L, name); /* ->TOS */
#if (0 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
}
/*
 *
 */
void lstateSetGlobalVal(lua_State *L, char *name, char *value)
{
    _setGlobalVal(L, name, value, NULL);
}
/*
 *
 */
void lstateSetGlobalValL(lua_State *L, char *name, char *value, int vlen)
{
    _setGlobalVal(L, name, value, &vlen);
}
/*
 * Compare two global values using different compare strategies.
 *
 * RETURN
 *     1 if values are matches, 0 if values does not matches or one of
 *     value is nil.
 */
int lstateCompareGlobalVals(lua_State *L,
        char *name1, char *name2, enum lstate_compare_type_t type)
{
    int r, ret;

    ret = 0;

    r = lua_getglobal(L, name1); /* [val1]->TOS */
    if (r == LUA_TNIL)
    {
        lua_pop(L, 1); /* ->TOS */
        return 0;
    }
    r = lua_getglobal(L, name2); /* [val1][val2]->TOS */
    if (r == LUA_TNIL)
    {
        lua_pop(L, 2); /* ->TOS */
        return 0;
    }

    if (type == LSTATE_COMPARE_LUA_EQUAL)
    {
        if (lua_compare(L, -1, -2, LUA_OPEQ))
            ret = 1;
    } else if (type == LSTATE_COMPARE_CASE || LSTATE_COMPARE_NOCASE) {
        const char *val1;
        const char *val2;

        val1 = lua_tostring(L, -2);
        val2 = lua_tostring(L, -1);
        if (val1 && val2)
        {
            if (type == LSTATE_COMPARE_CASE)
            {
                if (strcmp(val1, val2) == 0)
                    ret = 1;
            } else if (type == LSTATE_COMPARE_NOCASE) {
                if (strcasecmp(val1, val2) == 0)
                    ret = 1;
            }
        }
    }
    lua_pop(L, 2); /* ->TOS */
#if (0 && (defined DEBUG_THIS))
    /* Stack MUST be empty (gettop return 0). */
    debugPrint(DLEVEL_NOISE, "%s stack \"%d\" [sock %d].",
            __FUNCTION__, lua_gettop(L), client->sock);
#endif
    return ret;
}

///*
// * Print full table of client's request storead in lua state. Used for debug
// * purposes.
// */
//void lstatePrintRequest(lua_State *L)
//{
//    static char script[] = {
//        "function print_table(level, tab)" NL
//        "    if not tab then" NL
//        "        return" NL
//        "    end" NL
//        "    for k, v in pairs(tab) do" NL
//        "        if type(v) == \"string\" or type(v) == \"table\" then" NL
//        "            print(level, string.rep('  ', level) .. k, v, '# ' .. #v)" NL
//        "        else" NL
//        "            print(level, string.rep('  ', level) .. k, v)" NL
//        "        end" NL
//        "        if type(v) == \"table\" then" NL
//        "            print_table(level + 1, v)" NL
//        "        end" NL
//        "    end" NL
//        "end" NL
//        "print_table(1, request)" NL
//    };
//    if (luaL_dostring(L, script))
//    {
//        debugPrint(DLEVEL_NOISE, "Failed to execute script: \"%s\"." NL,
//                lua_tostring(L, -1));
//        lua_pop(L, 1);
//    }
//}
