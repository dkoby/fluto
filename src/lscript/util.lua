----
--
--
util = {}
----
--
--
function util.fileExists(path)
    local function tryToOpen(path)
        fd = io.open(path, "r")
        fd:close()
    end
    local ok, msg = pcall(tryToOpen, path)
    if not ok then
        return false
    else
        return true
    end
end
----
--
--
function util.fileSize(path)
    local fd = io.open(path, "r")
    local size = fd:seek("end")
    fd:close()
    return size
end
----
--
--
function util.errorResponse(errCode, short)
    local errFile = "error/error" .. errCode .. ".lua"
    if short or not util.fileExists(errFile) then
        response:send(errCode, 0)
    else
        local ok, msg = pcall(dofile, errFile)
        if not ok then
            debugPrint(DLEVEL_NOISE, "(E) failed to execute error file: " .. msg)
        end
    end
end
----
--
--
function util.outputFile(path, contentType)
    if not util.fileExists(path) then
        util.errorResponse(HTTP_ERROR_404_NOT_FOUND)
        return
    end

    response.charset = "UTF-8"
    response.contentType = contentType

    response:send(HTTP_ERROR_200_OK, util.fileSize(path))
    local fd = io.open(path, "rb")
    response:sendContent(fd:read("a"))
    fd:close()
end
----
--
--
function util.debugPrint(level, ...)
    local s = ""
    for i, v in ipairs({...}) do
        s = s .. v
    end
    debugPrint(level, s)
end
-------------------------------------------------------------------------------
-- String buffer for ease of output.
-------------------------------------------------------------------------------
util.StringBuilder = {}
function util.StringBuilder:new()
    local obj = {}

    setmetatable(obj, self)
    self.__index = self

    obj.val = {}
    return obj
end
function util.StringBuilder:append(...)
    for i, v in ipairs({...}) do
        table.insert(self.val, v)
    end
end
function util.StringBuilder:appendn(...)
    self:append(...)
    table.insert(self.val, v)
end
function util.StringBuilder:get()
    return table.concat(self.val)
end
-------------------------------------------------------------------------------

