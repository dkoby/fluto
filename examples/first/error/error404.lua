
debugPrint(DLEVEL_NOISE, "ERROR 404, " .. request.path)

if true then
    local sb = util.StringBuilder:new()

    sb:appendn("<!DOCTYPE html>");
    sb:appendn("<html>")
    sb:appendn("<head>")
    sb:appendn('<meta charset="UTF-8"/>')
    sb:appendn("<style>")
    sb:appendn("body {")
    sb:appendn("    background-color : #222;")
    sb:appendn("    color            : #c88;")
    sb:appendn("}")
    sb:appendn("</style>")
    sb:appendn("<title>")
    sb:appendn(HTTP_ERROR_404_NOT_FOUND, " error example.")
    sb:appendn("</title>")
    sb:appendn("</head>")
    sb:appendn('<body>')
    sb:appendn("<h1>Not Found</h1>")
    sb:appendn("<h2>", request.path, "</h2>")
    sb:appendn("<hr>")
    sb:appendn("</body>")
    sb:appendn("</html>")

    response:sendFull(HTTP_ERROR_404_NOT_FOUND, sb:get())
end

