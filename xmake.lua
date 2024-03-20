local sources = {
    "src/sqlite/sqlite3.c",
    "src/module.cpp"
}

target("SQLModule")
    set_kind("shared")
    add_files(sources)
    add_includedirs("include")
    set_pcxxheader("src/pch.hpp")

    if is_plat("windows") then
        add_defines("WIN32")
    end