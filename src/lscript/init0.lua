----
--
--
request  = {}
response = {}
----
-- Send headers, contentLength field and optionally content
--
function response:send(errCode, contentLength, content)
    if not self.contentType then
        self.contentType = "text/html"
    end
    if not self.charset then
        self.charset = "UTF-8"
    end

    local r = ""
    local append = function(data)
        r = r .. data
    end

    if not errCode or errCode == HTTP_ERROR_403_FORBIDDEN then
        append("HTTP/1.1 403 Forbidden\r\n")
    elseif errCode == HTTP_ERROR_200_OK then
        append("HTTP/1.1 200 OK\r\n")
    elseif errCode == HTTP_ERROR_400_BAD_REQUEST then
        append("HTTP/1.1 400 Bad Request\r\n")
    elseif errCode == HTTP_ERROR_404_NOT_FOUND then
        append("HTTP/1.1 404 Not Found\r\n")
    else
        append("HTTP/1.1 403 Forbidden\r\n")
    end
    append("Server: Fluto\r\n")

    append("Content-type: " .. self.contentType)
    if self.charset then
        append("; " .. self.charset)
    end
    append("\r\n")

    if contentLength then
        append("Content-length: " .. contentLength .. "\r\n")
    end

    if request.keepAlive then
        append("Connection: keep-alive\r\n")
    else
        append("Connection: close\r\n")
    end
    append("\r\n")

    self.writeSock(r)

    if content then
        self.writeSock(content)
    end
end
----
-- Send full response with content
--
function response:sendFull(errCode, content)
    self:send(errCode, #content, content)
end
----
-- Send content only. Headers must be sended before with "send" function.
--
function response:sendContent(content)
    self.writeSock(content)
end
----
-- Execute "config.lua" file in root of web-server working directory.
--
if true then
    local configFile = "config.lua"
    if util.fileExists(configFile) then
        local ok, msg = pcall(dofile, configFile)
        if not ok then
            debugPrint(DLEVEL_NOISE, "(E) failed to execute " .. configFile .. ": " .. msg)
        end
    end
end

