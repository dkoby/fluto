----
--
--
if true then
    local sb = util.StringBuilder:new()

    sb:appendn('<!DOCTYPE html>');
    sb:appendn('<html>')
    sb:appendn('<head>')
    sb:appendn('<meta charset="UTF-8"/>')
    sb:appendn('<link rel="stylesheet" href="css/style.css">')
    sb:appendn('<title>First example</title>')
    sb:appendn('</head>')
    sb:appendn('<body>')
    sb:appendn('<div>')
    sb:appendn('There was <span id="jsCounter">-</span> requests on keep-alive connection.')
    sb:appendn('</div>')
    sb:appendn('<div id="requestError">')
    sb:appendn('</div>')
    sb:appendn('<div><a href="index.html?param1=value1&param2=value2&param3">Test of query string 1</a></div>')
    sb:appendn('<div><a href="index.html?param1=value_Of_parameter_1">Test of query string 2</a></div>')
    --
    -- Full query string is contained in "request.query"
    -- Individual parameters can be retrieved from "request.qtable" table.
    --
    if request.qtable then
        sb:appendn('<table class="queryTable">')
        sb:appendn('<caption>Query arguments</caption>')
        sb:appendn('<tr><th>Name</th><th>Value</th></tr>')
        for k,v in pairs(request.qtable) do
            sb:appendn('<tr><td>', k, '</td><td>', v, '</td></tr>')
        end
        sb:appendn('</table>')
    end
    sb:appendn('<script src="js/script.js"></script>')
    sb:appendn('</body>')
    sb:appendn('</html>')

    response:sendFull(HTTP_ERROR_200_OK, sb:get())
end

