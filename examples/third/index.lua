----
--
--
if true then
    local s = util.stringBuf:new()

    s:pn('<!DOCTYPE html>')
    s:pn('<html>')
    s:pn('<head>')
    s:pn('<meta charset="UTF-8"/>')
    s:pn('<link rel="stylesheet" href="style.css">')
    s:pn('<title>Third example</title>')
    s:pn('</head>')
    s:pn('<body>')

    local db = sqlite3.open_memory()

    db:exec[[
        CREATE TABLE test (id INTEGER PRIMARY KEY, content);

        INSERT INTO test VALUES (NULL, 'Hello World');
        INSERT INTO test VALUES (NULL, 'Hello Lua');
        INSERT INTO test VALUES (NULL, 'Hello Sqlite3')
    ]]
    s:pn('<table>');
    for row in db:nrows("SELECT * FROM test") do
        s:pn('<tr><td>', row.id, '</td><td>', row.content, '</td></tr>')
    end    
    s:pn('</table>');

    s:pn('</body>')
    s:pn('</html>')

    db:close()
    response:sendFull(HTTP_ERROR_200_OK, s:get())
end

