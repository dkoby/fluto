if true then
    local s = util.stringBuf:new()

    --
    -- Global variable. Preserved on keep-alive connection.
    --
    if not counter then
        counter = 0
    end
    counter = counter + 1

    s:pn('{')
    s:pn('"result" : "', counter, '"')
    s:pn('}')

    response.contentType = "application/json"
    response:sendFull(HTTP_ERROR_200_OK, s:get())
end

