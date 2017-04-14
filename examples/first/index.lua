----
--
--
if true then
    local s = util.stringBuf:new()

    s:pn('<!DOCTYPE html>');
    s:pn('<html>')
    s:pn('<head>')
    s:pn('<meta charset="UTF-8"/>')
    s:pn('<link rel="stylesheet" href="css/style.css">')
    s:pn('<title>First example</title>')
    s:pn('</head>')
    s:pn('<body>')
    s:pn('<div>')
    s:pn('There was <span id="jsCounter">-</span> requests on keep-alive connection.')
    s:pn('</div>')
    s:pn('<div id="requestError">')
    s:pn('</div>')
    s:pn('<div><a href="index.html?param1=value1&param2=value2&param3">Test of query string 1</a></div>')
    s:pn('<div><a href="index.html?param1=value_Of_parameter_1">Test of query string 2</a></div>')
    --
    -- Full query string is contained in "request.query"
    -- Individual parameters can be retrieved from "request.qtable" table.
    --
    if request.qtable then
        s:pn('<table class="queryTable">')
        s:pn('<caption>Query arguments</caption>')
        s:pn('<tr><th>Name</th><th>Value</th></tr>')
        for k,v in pairs(request.qtable) do
            s:pn('<tr><td>', k, '</td><td>', v, '</td></tr>')
        end
        s:pn('</table>')
    end
    s:pn('<script src="js/script.js"></script>')
    s:pn('</body>')
    s:pn('</html>')

    response:sendFull(HTTP_ERROR_200_OK, s:get())
end

