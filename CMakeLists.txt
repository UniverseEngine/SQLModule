set(SOURCES
    "src/sqlite/sqlite3.c"
    "src/pch.cpp"
    "src/module.cpp"
)

add_library(SQLModule SHARED ${SOURCES})
target_include_directories(SQLModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(SQLModule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)
set_target_properties(SQLModule PROPERTIES PREFIX "")

install(TARGETS SQLModule RUNTIME DESTINATION "Server/modules" COMPONENT LCMPServer)