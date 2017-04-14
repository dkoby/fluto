
----
--
--
function process()
    if httpError ~= HTTP_ERROR_200_OK then
        util.errorResponse(httpError)
        return
    end
    --
    -- config.lua not allowed.
    --
    if request.path == "/config.lua" then
        util.errorResponse(HTTP_ERROR_404_NOT_FOUND)
        return
    end
    --
    -- First lookup in "handler" table for appropriate processor.
    --
    local handlerMatch = false
    if handler then
        for pattern, path in pairs(handler) do
            if string.match(request.path, pattern) then
                handlerMatch = true
                local ok, msg = pcall(dofile, path)
                if not ok then
                    debugPrint(DLEVEL_NOISE, "(E) failed to execute file " .. path .. ": " .. msg)
                end
                return
            end
        end
    end
    --
    -- Lookup for common files.
    --
    local path
    if not handlerMatch then
        path = string.match(request.path, "/(.*%.html)$")
        if path then
            util.outputFile(path, "text/html")
            return
        end
        path = string.match(request.path, "/(.*%.css)$")
        if path then
            util.outputFile(path, "text/css")
            return
        end
        path = string.match(request.path, "/(.*%.js)$")
        if path then
            util.outputFile(path, "application/x-javascript")
            return
        end
        path = string.match(request.path, "/(.*%.ico)$")
        if path then
            util.outputFile(path, "image/vnd.microsoft.icon")
            return
        end
    end
    --
    -- Not found.
    --
    util.errorResponse(HTTP_ERROR_404_NOT_FOUND)
end

process()

