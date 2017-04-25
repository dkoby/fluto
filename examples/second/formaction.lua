if true then
    local sb = util.StringBuilder:new()

    if request.contentType then
        util.debugPrint(DLEVEL_NOISE, "contentType: " .. request.contentType);
        if request.contentLength then
            local n = request.contentLength
            while n > 0 do
                local chunkSize = 10

                if chunkSize > n then
                    chunkSize = n
                end

                local chunk = request.getContent(chunkSize)
                if #chunk == 0 then
                    break
                end
                util.debugPrint(DLEVEL_NOISE, 'content: "', chunk, '"')
                n = n - #chunk
                util.debugPrint(DLEVEL_NOISE, 'remain: ', n)
            end
        end
    end
    util.debugPrint(DLEVEL_NOISE, 'done')

    sb:appendn('{')
    sb:appendn('"result" : "ok"')
    sb:appendn('}')

    response.contentType = "application/json"
    response:sendFull(HTTP_ERROR_200_OK, sb:get())
end

