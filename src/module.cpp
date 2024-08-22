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

                    if ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
                    {
                        info.GetReturnValue().Set((void*)stmt);
                        return;
                    }

                    info.GetVM()->ThrowException("[sqlmodule] Error in query: " + String(sqlite3_errmsg(db)));

                    sqlite3_finalize(stmt);
                });

                sqldatabase.SetFunction("close", [](Scripting::API::ICallbackInfo& info) {
                    sqlite3_close_v2((sqlite3*)info.This().GetInternal());
                });
            }

            info.GetReturnValue().Set(sqldatabase);
        });

        vm->RegisterGlobalFunction("sqlite3_reset", [](Scripting::API::ICallbackInfo& info) {
            sqlite3_reset((sqlite3_stmt*)info[0].ToExternal());
        });

        vm->RegisterGlobalFunction("sqlite3_column_count", [](Scripting::API::ICallbackInfo& info) {
            info.GetReturnValue().Set(sqlite3_column_count((sqlite3_stmt*)info[0].ToExternal()));
        });

        vm->RegisterGlobalFunction("sqlite3_column_data", [](Scripting::API::ICallbackInfo& info) {
            auto stmt   = (sqlite3_stmt*)info[0].ToExternal();
            int  column = info[1].ToNumber();
            switch (sqlite3_column_type(stmt, column))
            {
            case SQLITE_INTEGER:
                info.GetReturnValue().Set(sqlite3_column_int(stmt, column));
                break;
            case SQLITE_FLOAT:
                info.GetReturnValue().Set(sqlite3_column_double(stmt, column));
                break;
            case SQLITE_TEXT:
                info.GetReturnValue().Set(String((const char*)sqlite3_column_text(stmt, column)));
                break;
            case SQLITE_BLOB:
                info.GetVM()->ThrowException("[sqlmodule] no blob support");
            default:
                info.GetReturnValue().SetNull();
                break;
            }
        });

        vm->RegisterGlobalFunction("sqlite3_column_name", [](Scripting::API::ICallbackInfo& info) {
            auto stmt   = (sqlite3_stmt*)info[0].ToExternal();
            int  column = info[1].ToNumber();
            if (column > sqlite3_column_count(stmt))
            {
                info.GetVM()->ThrowException("column number exceeded column count");
                return;
            }
            if (column < 0)
            {
                info.GetVM()->ThrowException("column number can't be negative");
                return;
            }
            info.GetReturnValue().Set(String(sqlite3_column_name(stmt, column)));
        });

        vm->RegisterGlobalFunction("sqlite3_column_decltype", [](Scripting::API::ICallbackInfo& info) {
            auto stmt   = (sqlite3_stmt*)info[0].ToExternal();
            int  column = info[1].ToNumber();
            if (column > sqlite3_column_count(stmt))
            {
                info.GetVM()->ThrowException("column number exceeded column count");
                return;
            }
            if (column < 0)
            {
                info.GetVM()->ThrowException("column number can't be negative");
                return;
            }
            info.GetReturnValue().Set(String(sqlite3_column_decltype(stmt, column)));
        });

        vm->RegisterGlobalFunction("sqlite3_finalize", [](Scripting::API::ICallbackInfo& info) {
            sqlite3_finalize((sqlite3_stmt*)info[0].ToExternal());
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
