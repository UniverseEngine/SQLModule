#include "module.hpp"

#include <sqlite/sqlite3.h>

#include <stdexcept>
#include <string>

// TODO: Refactor when ScriptingAPI is updated

using namespace Universe::Scripting::API;

const static std::string NAME    = "SQLite Module";
const static std::string DESC    = "Provides SQLite functionality";
const static std::string VERSION = "1.0.0-rc.1";
const static std::string AUTHOR  = "lucx, perikiyoxd";

// Module API
static IModuleInterface* moduleInterfaceInstance = nullptr;

// Scripting API
static IClassDefinition* sqlDatabaseClass    = nullptr;
static IClassDefinition* sqlQueryResultClass = nullptr;
static IClassDefinition* sqlQueryRowClass    = nullptr;

// Module API functions
void AnnounceModule::ModuleHandler::OnModuleLoad(ModuleDetails& details, IModuleInterface& moduleInterface)
{
    details.moduleName        = NAME;
    details.moduleDescription = DESC;
    details.moduleAuthor      = VERSION;
    details.moduleVersion     = AUTHOR;

    moduleInterfaceInstance = &moduleInterface;
}

// Scripting API functions
static sqlite3* getDbFromInfo(ICallbackInfo& info)
{
    auto db = static_cast<sqlite3*>(info.This().GetInternal());
    if (!db)
    {
        info.GetVM()->ThrowException("[sqlmodule] Database not open");
    }
    return db;
}

static void finalizeStmt(sqlite3_stmt*& stmt)
{
    sqlite3_finalize(stmt);
}

static void setColumnValue(
    IObject*           obj,
    const std::string& colname,
    int                coltype,
    sqlite3_stmt*      stmt,
    int                col)
{
    switch (coltype)
    {
    case SQLITE_INTEGER:
        obj->Set(colname, sqlite3_column_int(stmt, col));
        break;
    case SQLITE_FLOAT:
        obj->Set(colname, sqlite3_column_double(stmt, col));
        break;
    case SQLITE_BLOB:
        obj->SetNull(colname);
        break;
    case SQLITE3_TEXT:
        obj->Set(colname, std::string { reinterpret_cast<const char*>(sqlite3_column_text(stmt, col)) });
        break;
    case SQLITE_NULL:
        obj->SetNull(colname);
        break;
    default:
        break;
    }
}

static void populateColumns(
    IObject*      sqlStatementObject,
    sqlite3_stmt* stmt)
{
    for (int col = 0; col < sqlite3_column_count(stmt); col++)
    {
        std::string colname { sqlite3_column_name(stmt, col) };
        int         coltype = sqlite3_column_type(stmt, col);
        setColumnValue(sqlStatementObject, colname, coltype, stmt, col);
    }
}

static ::IObject* populateRow(
    ICallbackInfo& info,
    sqlite3_stmt*  stmt)
{
    IObject* objStmt2 = &info.ObjectValue("QueryRow", nullptr);
    populateColumns(objStmt2, stmt);
    return objStmt2;
}

static std::string escapeString(sqlite3* db, const std::string& input)
{
    const char* escapedString = sqlite3_mprintf("%q", input.c_str());
    if (!escapedString)
    {
        throw std::runtime_error("Memory allocation failed during string escaping");
    }
    std::string result { escapedString };
    sqlite3_free(const_cast<char*>(escapedString));
    return result;
}

void AnnounceModule::ScriptingHandler::OnRegister(IVM* vm)
{
    auto& globalCtx = vm->Global();

    // Register constants
    globalCtx.Set("SQLITE_OPEN_READWRITE", SQLITE_OPEN_READWRITE);
    globalCtx.Set("SQLITE_OPEN_CREATE", SQLITE_OPEN_CREATE);
    globalCtx.Set("SQLITE_OPEN_DELETEONCLOSE", SQLITE_OPEN_DELETEONCLOSE);
    globalCtx.Set("SQLITE_OPEN_EXCLUSIVE", SQLITE_OPEN_EXCLUSIVE);
    globalCtx.Set("SQLITE_OPEN_AUTOPROXY", SQLITE_OPEN_AUTOPROXY);
    globalCtx.Set("SQLITE_OPEN_URI", SQLITE_OPEN_URI);
    globalCtx.Set("SQLITE_OPEN_MEMORY", SQLITE_OPEN_MEMORY);
    globalCtx.Set("SQLITE_OPEN_MAIN_DB", SQLITE_OPEN_MAIN_DB);
    globalCtx.Set("SQLITE_OPEN_TEMP_DB", SQLITE_OPEN_TEMP_DB);
    globalCtx.Set("SQLITE_OPEN_TRANSIENT_DB", SQLITE_OPEN_TRANSIENT_DB);
    globalCtx.Set("SQLITE_OPEN_MAIN_JOURNAL", SQLITE_OPEN_MAIN_JOURNAL);
    globalCtx.Set("SQLITE_OPEN_TEMP_JOURNAL", SQLITE_OPEN_TEMP_JOURNAL);
    globalCtx.Set("SQLITE_OPEN_SUBJOURNAL", SQLITE_OPEN_SUBJOURNAL);
    globalCtx.Set("SQLITE_OPEN_SUPER_JOURNAL", SQLITE_OPEN_SUPER_JOURNAL);
    globalCtx.Set("SQLITE_OPEN_NOMUTEX", SQLITE_OPEN_NOMUTEX);
    globalCtx.Set("SQLITE_OPEN_FULLMUTEX", SQLITE_OPEN_FULLMUTEX);
    globalCtx.Set("SQLITE_OPEN_SHAREDCACHE", SQLITE_OPEN_SHAREDCACHE);
    globalCtx.Set("SQLITE_OPEN_PRIVATECACHE", SQLITE_OPEN_PRIVATECACHE);
    globalCtx.Set("SQLITE_OPEN_WAL", SQLITE_OPEN_WAL);
    globalCtx.Set("SQLITE_OPEN_NOFOLLOW", SQLITE_OPEN_NOFOLLOW);
    globalCtx.Set("SQLITE_OPEN_EXRESCODE", SQLITE_OPEN_EXRESCODE);

    // Register functions
    vm->RegisterGlobalFunction("open", open);
    vm->RegisterGlobalFunction("escape", escape);

    // Register classes
    auto& classRegistry = vm->GetClassRegistry();

    sqlDatabaseClass = classRegistry.RegisterClass("SQLDatabase");
    {
        sqlDatabaseClass->SetFunction("exec", exec);
        sqlDatabaseClass->SetFunction("query", query);
        sqlDatabaseClass->SetFunction("close", close);
    }

    sqlQueryResultClass = classRegistry.RegisterClass("SQLiteQueryResult");
    {
        sqlQueryResultClass->SetProperty("rows", nullptr);
        sqlQueryResultClass->SetProperty("rowCount", 0);
    }

    sqlQueryRowClass = classRegistry.RegisterClass("SQLiteQueryRow");
}

void AnnounceModule::ScriptingHandler::open(ICallbackInfo& info)
{
    std::string filename = info[0].ToString();
    int         flags    = info[1].ToNumber();
    std::string zVfs     = "";
    if (info.Length() - 1 > 1)
        zVfs = info[2].ToString();

    sqlite3* db;
    sqlite3_open_v2(filename.c_str(), &db, flags, zVfs.empty() ? 0 : zVfs.c_str());

    IObject& sqldatabase = sqlDatabaseClass->CreateInstance({ { "db", db } });
    info.GetReturnValue().Set(sqldatabase);
}

void AnnounceModule::ScriptingHandler::exec(ICallbackInfo& info)
{
    sqlite3* db = getDbFromInfo(info);

    int ret = sqlite3_exec(db, info[0].ToString().c_str(), 0, 0, 0);
    if (ret != SQLITE_OK)
    {
        info.GetVM()->ThrowException("[sqlmodule] Error in exec: " + std::string(sqlite3_errmsg(db)));
        return;
    }

    info.GetReturnValue().Set(true);
}

void AnnounceModule::ScriptingHandler::query(ICallbackInfo& info)
{
    sqlite3* db = getDbFromInfo(info);

    sqlite3_stmt* stmt;

    int ret = sqlite3_prepare_v3(db, info[0].ToString().c_str(), -1, 0, &stmt, 0);
    if (ret != SQLITE_OK)
    {
        info.GetVM()->ThrowException("[sqlmodule] Error in query: " + std::string(sqlite3_errmsg(db)));
        sqlite3_finalize(stmt);
        return;
    }

    auto& objStmt = info.ObjectValue("SQLQueryResult", nullptr);
    int   count   = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto row = populateRow(info, stmt);
        objStmt.Set("rows", row); // Assuming the caller handles "rows" being overwritten or needs batching logic here
        count++;
    }

    objStmt.Set("rowCount", count);

    finalizeStmt(stmt);
    info.GetReturnValue().Set(objStmt);
}

void AnnounceModule::ScriptingHandler::close(ICallbackInfo& info)
{
    sqlite3* db = getDbFromInfo(info);

    sqlite3_close(db);

    info.GetReturnValue().Set(true);
}

void AnnounceModule::ScriptingHandler::escape(ICallbackInfo& info)
{
    sqlite3* db = getDbFromInfo(info);

    try
    {
        info.GetReturnValue().Set(escapeString(db, info[0].ToString()));
    }
    catch (const std::exception& e)
    {
        info.GetVM()->ThrowException("[sqlmodule] Error in escape: " + std::string(e.what()));
    }
}

// Entry point
ModuleAPIExport IModuleHandler* CreateModuleHandler()
{
    return AnnounceModule::m_moduleHandler.get();
}
