#include "module.hpp"

#include <sqlite/sqlite3.h>

const static std::string NAME    = "SQLite Module";
const static std::string DESC    = "Provides SQLite functionality";
const static std::string VERSION = "1.0.0-rc.1";
const static std::string AUTHOR  = "lucx, perikiyoxd";

static IModuleInterface* m_moduleInterface;

void AnnounceModule::ModuleHandler::OnModuleLoad(ModuleDetails& details, IModuleInterface* moduleInterface)
{
    details.moduleName        = NAME;
    details.moduleDescription = DESC;
    details.moduleAuthor      = VERSION;
    details.moduleVersion     = AUTHOR;

    m_moduleInterface = moduleInterface;
}

void AnnounceModule::ModuleHandler::OnModuleTick() {}

void RegisterFunctions(Universe::Scripting::API::IVM* vm)
{
    auto& globalCtx = vm->Global();

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

    vm->RegisterGlobalFunction("sqlite3_open", [](Universe::Scripting::API::ICallbackInfo& info) {
        std::string filename = info[0].ToString();
        int         flags    = info[1].ToNumber();
        std::string zVfs     = "";
        if (info.Length() - 1 > 1)
            zVfs = info[2].ToString();

        sqlite3* db;
        sqlite3_open_v2(filename.c_str(), &db, flags, zVfs.empty() ? 0 : zVfs.c_str());

        auto& sqldatabase = info.ObjectValue("SqlDatabase", db);
        {
            sqldatabase.SetFunction("exec", [](Universe::Scripting::API::ICallbackInfo& info) {
                char* errmsg = 0;
                sqlite3_exec(static_cast<sqlite3*>(info.This().GetInternal()), info[0].ToString().c_str(), 0, 0, &errmsg);

                if (errmsg)
                    info.GetVM()->ThrowException("[sqlmodule] Error executing: " + std::string(errmsg));
            });

            sqldatabase.SetFunction("queryOne", [](Universe::Scripting::API::ICallbackInfo& info) {
                sqlite3* db = static_cast<sqlite3*>(info.This().GetInternal());

                sqlite3_stmt* stmt;

                int ret = sqlite3_prepare_v3(db, info[0].ToString().c_str(), -1, 0, &stmt, 0);
                if (ret != SQLITE_OK)
                {
                    info.GetVM()->ThrowException("[sqlmodule] Error in query: " + std::string(sqlite3_errmsg(db)));
                    sqlite3_finalize(stmt);
                    return;
                }

                auto& objStmt = info.ObjectValue("sqlStmt", nullptr);

                ret = sqlite3_step(stmt);

                for (int col = 0; col < sqlite3_column_count(stmt); col++)
                {
                    std::string colname { sqlite3_column_name(stmt, col) };

                    int coltype = sqlite3_column_type(stmt, col);
                    switch (sqlite3_column_type(stmt, col))
                    {
                    case SQLITE_INTEGER:
                        objStmt.Set(colname, sqlite3_column_int(stmt, col));
                        break;
                    case SQLITE_FLOAT:
                        objStmt.Set(colname, sqlite3_column_double(stmt, col));
                        break;
                    case SQLITE_BLOB:
                        // not supported, set it to null
                        objStmt.SetNull(colname);
                        break;
                    case SQLITE3_TEXT:
                        objStmt.Set(colname, sqlite3_column_text(stmt, col));
                        break;
                    case SQLITE_NULL:
                        objStmt.SetNull(colname);
                        break;
                    default:
                        break;
                    }
                }

                if (ret == SQLITE_ROW)
                    info.GetReturnValue().Set(objStmt);
                else
                    info.GetReturnValue().SetNull();

                sqlite3_finalize(stmt);
            });

            sqldatabase.SetFunction("query", [](Universe::Scripting::API::ICallbackInfo& info) {
                sqlite3* db = static_cast<sqlite3*>(info.This().GetInternal());

                sqlite3_stmt* stmt;

                int ret = sqlite3_prepare_v3(db, info[0].ToString().c_str(), -1, 0, &stmt, 0);
                if (ret != SQLITE_OK)
                {
                    info.GetVM()->ThrowException("[sqlmodule] Error in query: " + std::string(sqlite3_errmsg(db)));
                    sqlite3_finalize(stmt);
                    return;
                }

                auto& objStmt = info.ObjectValue("QueryResult", nullptr);

                int count {};
                while (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    auto& objStmt2 = info.ObjectValue("QueryRow", nullptr);

                    for (int col = 0; col < sqlite3_column_count(stmt); col++)
                    {
                        std::string colname { sqlite3_column_name(stmt, col) };

                        int coltype = sqlite3_column_type(stmt, col);
                        switch (sqlite3_column_type(stmt, col))
                        {
                        case SQLITE_INTEGER:
                            objStmt2.Set(colname, sqlite3_column_int(stmt, col));
                            break;
                        case SQLITE_FLOAT:
                            objStmt2.Set(colname, sqlite3_column_double(stmt, col));
                            break;
                        case SQLITE_BLOB:
                            // not supported, set it to null
                            objStmt2.SetNull(colname);
                            break;
                        case SQLITE3_TEXT:
                            objStmt2.Set(colname, std::string { reinterpret_cast<const char*>(sqlite3_column_text(stmt, col)) });
                            break;
                        case SQLITE_NULL:
                            objStmt2.SetNull(colname);
                            break;
                        default:
                            break;
                        }
                    }

                    objStmt.Set("rows", objStmt2);

                    count++;
                }
                objStmt.Set("rowCount", count);
                sqlite3_finalize(stmt);
                info.GetReturnValue().Set(objStmt);
            });

            sqldatabase.SetFunction("close", [](Universe::Scripting::API::ICallbackInfo& info) {
                sqlite3_close_v2(static_cast<sqlite3*>(info.This().GetInternal()));
            });
        }

        info.GetReturnValue().Set(sqldatabase);
    });

    vm->RegisterGlobalFunction("sqlite3_escape", [](Universe::Scripting::API::ICallbackInfo& info) {
        std::string str = sqlite3_mprintf("%q", info[0].ToString().c_str());
        info.GetReturnValue().Set(str);
    });
}

extern "C" DLLEXPORT IModuleHandler* CreateModuleHandler()
{
    return AnnounceModule::m_moduleHandler.get();
}
