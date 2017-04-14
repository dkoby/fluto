----
--
--
if true then
    local getContent = request.getContent

    request  = {}
    request.getContent = getContent
end

response.charset     = "UTF-8"
response.contentType = "text/html"

