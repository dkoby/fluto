----
--
--
if true then
    local sb = util.StringBuilder:new()

    sb:appendn('<!DOCTYPE html>')
    sb:appendn('<html>')
    sb:appendn('<head>')
    sb:appendn('<meta charset="UTF-8"/>')
    sb:appendn('<link rel="stylesheet" href="style.css">')
    sb:appendn('<title>Third example</title>')
    sb:appendn('</head>')
    sb:appendn('<body>')

    local db = sqlite3.open_memory()

    db:exec[[
        CREATE TABLE test (id INTEGER PRIMARY KEY, content);

        INSERT INTO test VALUES (NULL, 'Hello World');
        INSERT INTO test VALUES (NULL, 'Hello Lua');
        INSERT INTO test VALUES (NULL, 'Hello Sqlite3')
    ]]
    sb:appendn('<table>');
    for row in db:nrows("SELECT * FROM test") do
        sb:appendn('<tr><td>', row.id, '</td><td>', row.content, '</td></tr>')
    end    
    sb:appendn('</table>');

    sb:appendn('</body>')
    sb:appendn('</html>')

    db:close()
    response:sendFull(HTTP_ERROR_200_OK, sb:get())
end

