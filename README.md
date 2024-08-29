# SQLModule

## Code example

JavaScript
```javascript
addEventHandler("OnResourceStart", () => {
    const db = sqlite3_open("test.db", SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE);
    db.exec("CREATE TABLE IF NOT EXISTS test (id INT, model TEXT);");
    db.exec("INSERT INTO test VALUES (0, 'blabla')");
    const result = db.queryOne("SELECT * FROM test WHERE id = 0");
    if (result) {
        console.log("result: " + result.model);
    }
    else {
        console.error("no results to process!");
    }
    db.close();

    console.log(sqlite3_escape("this is an 'escaped 'string to prevent injection'"));
});
```

Lua
```lua
addEventHandler("OnResourceStart",
    function()
        local db = sqlite3_open("test.db", SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE)
        db:exec("CREATE TABLE IF NOT EXISTS test (id INT, model TEXT);")
        db:exec("INSERT INTO test VALUES (0, 'blabla')")
        local result = db:queryOne("SELECT * FROM test WHERE id = 0")
        if result then
            print("result: " .. query.model)
        else
            print("no results!")
        end
        db:close()

        print(sqlite3_escape("this is an 'escaped 'string to prevent injection'"))
    end
)
```
