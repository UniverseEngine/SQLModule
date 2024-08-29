#include "module.hpp"

#include <sqlite/sqlite3.h>

namespace module
{
    DLLEXPORT void OnLoad(String* name, String* description, String* author, ModuleAPI::IModuleAPI* api)
    {
        *name        = "SQL Module";
        *description = "";
        *author      = "lucx";

        m_api = api;
    }

    DLLEXPORT void RegisterFunctions(Scripting::API::IVM* vm)
    {
        vm->Global().Set("SQLITE_OPEN_READWRITE", SQLITE_OPEN_READWRITE);
        vm->Global().Set("SQLITE_OPEN_CREATE", SQLITE_OPEN_CREATE);
        vm->Global().Set("SQLITE_OPEN_DELETEONCLOSE", SQLITE_OPEN_DELETEONCLOSE);
        vm->Global().Set("SQLITE_OPEN_EXCLUSIVE", SQLITE_OPEN_EXCLUSIVE);
        vm->Global().Set("SQLITE_OPEN_AUTOPROXY", SQLITE_OPEN_AUTOPROXY);
        vm->Global().Set("SQLITE_OPEN_URI", SQLITE_OPEN_URI);
        vm->Global().Set("SQLITE_OPEN_MEMORY", SQLITE_OPEN_MEMORY);
        vm->Global().Set("SQLITE_OPEN_MAIN_DB", SQLITE_OPEN_MAIN_DB);
        vm->Global().Set("SQLITE_OPEN_TEMP_DB", SQLITE_OPEN_TEMP_DB);
        vm->Global().Set("SQLITE_OPEN_TRANSIENT_DB", SQLITE_OPEN_TRANSIENT_DB);
        vm->Global().Set("SQLITE_OPEN_MAIN_JOURNAL", SQLITE_OPEN_MAIN_JOURNAL);
        vm->Global().Set("SQLITE_OPEN_TEMP_JOURNAL", SQLITE_OPEN_TEMP_JOURNAL);
        vm->Global().Set("SQLITE_OPEN_SUBJOURNAL", SQLITE_OPEN_SUBJOURNAL);
        vm->Global().Set("SQLITE_OPEN_SUPER_JOURNAL", SQLITE_OPEN_SUPER_JOURNAL);
        vm->Global().Set("SQLITE_OPEN_NOMUTEX", SQLITE_OPEN_NOMUTEX);
        vm->Global().Set("SQLITE_OPEN_FULLMUTEX", SQLITE_OPEN_FULLMUTEX);
        vm->Global().Set("SQLITE_OPEN_SHAREDCACHE", SQLITE_OPEN_SHAREDCACHE);
        vm->Global().Set("SQLITE_OPEN_PRIVATECACHE", SQLITE_OPEN_PRIVATECACHE);
        vm->Global().Set("SQLITE_OPEN_WAL", SQLITE_OPEN_WAL);
        vm->Global().Set("SQLITE_OPEN_NOFOLLOW", SQLITE_OPEN_NOFOLLOW);
        vm->Global().Set("SQLITE_OPEN_EXRESCODE", SQLITE_OPEN_EXRESCODE);

        vm->RegisterGlobalFunction("sqlite3_open", [](Scripting::API::ICallbackInfo& info) {
            String filename = info[0].ToString();
            int    flags    = info[1].ToNumber();
            String zVfs     = "";
            if (info.Length() - 1 > 1)
                zVfs = info[2].ToString();

            sqlite3* db;
            sqlite3_open_v2(filename.c_str(), &db, flags, zVfs.empty() ? 0 : zVfs.c_str());

            auto& sqldatabase = info.ObjectValue("SqlDatabase", db);
            {
                sqldatabase.SetFunction("exec", [](Scripting::API::ICallbackInfo& info) {
                    char* errmsg = 0;
                    sqlite3_exec((sqlite3*)info.This().GetInternal(), info[0].ToString().c_str(), 0, 0, &errmsg);

                    if (errmsg)
                        info.GetVM()->ThrowException("[sqlmodule] Error executing: " + String(errmsg));
                });

                sqldatabase.SetFunction("queryOne", [](Scripting::API::ICallbackInfo& info) {
                    sqlite3* db = (sqlite3*)info.This().GetInternal();

                    sqlite3_stmt* stmt;

                    int ret = sqlite3_prepare_v3(db, info[0].ToString().c_str(), -1, 0, &stmt, 0);
                    if (ret != SQLITE_OK)
                    {
                        info.GetVM()->ThrowException("[sqlmodule] Error in query: " + String(sqlite3_errmsg(db)));
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

                sqldatabase.SetFunction("query", [](Scripting::API::ICallbackInfo& info) {
                    sqlite3* db = (sqlite3*)info.This().GetInternal();

                    sqlite3_stmt* stmt;

                    int ret = sqlite3_prepare_v3(db, info[0].ToString().c_str(), -1, 0, &stmt, 0);
                    if (ret != SQLITE_OK)
                    {
                        info.GetVM()->ThrowException("[sqlmodule] Error in query: " + String(sqlite3_errmsg(db)));
                        sqlite3_finalize(stmt);
                        return;
                    }

                    auto& objStmt = info.ObjectValue("sqlStmt", nullptr);

                    int count {};
                    while (sqlite3_step(stmt))
                    {
                        auto& objStmt2 = info.ObjectValue("sqlStmt", nullptr);

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
                                objStmt2.Set(colname, sqlite3_column_text(stmt, col));
                                break;
                            case SQLITE_NULL:
                                objStmt2.SetNull(colname);
                                break;
                            default:
                                break;
                            }
                        }

                        objStmt.Set(count, objStmt2);

                        count++;
                    }

                    sqlite3_finalize(stmt);

                    info.GetReturnValue().Set(objStmt);
                });

                sqldatabase.SetFunction("close", [](Scripting::API::ICallbackInfo& info) {
                    sqlite3_close_v2((sqlite3*)info.This().GetInternal());
                });
            }

            info.GetReturnValue().Set(sqldatabase);
        });

        vm->RegisterGlobalFunction("sqlite3_escape", [](Scripting::API::ICallbackInfo& info) {
            String str = sqlite3_mprintf("%q", info[0].ToString().c_str());
            info.GetReturnValue().Set(str);
        });
    }

    DLLEXPORT void OnPulse()
    {
    }
} // namespace module
