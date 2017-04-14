if true then
    local s = util.stringBuf:new()

    if request.contentType and string.match(request.contentType, 'multipart/.*') then
        if request.contentLength then
            local n = request.contentLength
            while n > 0 do
                local chunkSize = 10

                if chunkSize > n then
                    chunkSize = n
                end

                local s = request.getContent(chunkSize)
                if #s == 0 then
                    break
                end
                util.debugPrint(DLEVEL_NOISE, 'content: "', s, '"')
                n = n - #s
                util.debugPrint(DLEVEL_NOISE, 'remain: ', n)
            end
        end
    end
    util.debugPrint(DLEVEL_NOISE, 'done')

    s:pn('{')
    s:pn('"result" : "ok"')
    s:pn('}')

    response.contentType = "application/json"
    response:sendFull(HTTP_ERROR_200_OK, s:get())
end

