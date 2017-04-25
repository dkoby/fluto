----
--
--
if true then
    local sb = util.StringBuilder:new()

    sb:append([[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8"/>
<link rel="stylesheet" href="style.css">
<title>Second example</title>
</head>
<body>
<form id="testForm">
</form>
<input name="image" type="file" id="myFile" accept=".txt,.bin" form="testForm">
<input name="userText" type="text" form="testForm">
<button onclick="clickAction()" class="myButton">Confirm</button>
<div class="error" id="error"></div>
<script src="script.js"></script>
</body>
</html>
    ]]);

    response:sendFull(HTTP_ERROR_200_OK, sb:get())
end

