if true then
    local sb = util.StringBuilder:new()

    --
    -- Global variable. Preserved on keep-alive connection.
    --
    if not counter then
        counter = 0
    end
    counter = counter + 1

    sb:appendn('{')
    sb:appendn('"result" : "', counter, '"')
    sb:appendn('}')

    response.contentType = "application/json"
    response:sendFull(HTTP_ERROR_200_OK, sb:get())
end

