# SQLModule

## Code example

JavaScript
```javascript
addEventHandler("OnResourceStart", () => {
    let db = sqlite3_open("test.db", SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE);
    db.exec("CREATE TABLE IF NOT EXISTS test (id INT, model TEXT);");
    db.exec("INSERT INTO test VALUES (0, 'blabla')");
    let query = db.query("SELECT * FROM test");
    if (query)
    {
        console.log("count: " + sqlite3_column_count(query));
        console.log("name: " + sqlite3_column_name(query, 3));
        sqlite3_finalize(query);
    }
    db.close();

    console.log(sqlite3_escape("this is an 'escaped 'string to prevent injection'"));
});
```

Lua
```lua
addEventHandler("OnResourceStart",
    function()
        local db = sqlite3_open("test.db", SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE);
        db:exec("CREATE TABLE IF NOT EXISTS test (id INT, model TEXT);");
        db:exec("INSERT INTO test VALUES (0, 'blabla')");
        local query = db:query("SELECT * FROM test");
        if query then
            print("count: " .. sqlite3_column_count(query));
            print("name: " .. sqlite3_column_name(query, 1));
            sqlite3_finalize(query);
        end
        db:close();

        print(sqlite3_escape("this is an 'escaped 'string to prevent injection'"));
    end
)
```