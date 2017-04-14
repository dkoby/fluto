
debugPrint(DLEVEL_NOISE, "ERROR 404, " .. request.path)

if true then
    local s = {}
    local p   = util.sprint
    local pn  = util.sprintn
    local pf  = util.sprintf
    local pfn = util.sprintfnl

    pn (s, "<!DOCTYPE html>");
    pn (s, "<html>")
    pn (s, "<head>")
    pn (s, '<meta charset="UTF-8"/>')
    pn (s, "<title>")
    pf (s, "%d error example.", HTTP_ERROR_404_NOT_FOUND)
    pn (s, "</title>")
    pn (s, "</head>")
    pfn(s, '<body bgcolor="%s">', '#b44')
    pn (s, "<h1>Not Found</h1>")
    pn (s, "<h2>", request.path, "</h2>")
    pn (s, "<hr>")
    pn (s, "</body>")
    pn (s, "</html>")

    response:sendFull(HTTP_ERROR_404_NOT_FOUND, s.s)
end

