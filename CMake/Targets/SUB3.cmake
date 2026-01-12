message(NOTICE "Adding SUB3 executable target")

add_executable(SUB3)

target_compile_definitions(SUB3
        PUBLIC SUB_VERSION="${CMAKE_PROJECT_VERSION}"
        PUBLIC SUB_TDLIB_RECEIVE_TIMEOUT=5.0
)

target_link_libraries(SUB3
        PRIVATE Td::tdjson_static
        PRIVATE glaze::glaze
        PRIVATE rpc
        PRIVATE subprocess
)

target_include_directories(SUB3
        PUBLIC "${PREFIX}/src"
        PUBLIC "${PREFIX}/third_party/plog/include"
        PUBLIC "${PREFIX}/third_party/yamc/include"
        PUBLIC "${PREFIX}/third_party/cli/include"
        PUBLIC "${PREFIX}/third_party/subprocess/src/cpp"
)

file(GLOB SUB3_src
        "${PREFIX}/main.cpp"
        "${PREFIX}/src/CLI/*.cc"
        "${PREFIX}/src/Config/*.cc"
        "${PREFIX}/src/Error/*.cc"
        "${PREFIX}/src/Main/*.cc"
        "${PREFIX}/src/Safety/*.cc"
        "${PREFIX}/src/Server/RPC/*.cc"
        "${PREFIX}/src/Td/*.cc"
)

target_sources(SUB3
        PUBLIC ${SUB3_src}
)

target_sources(SUB3
        PRIVATE
        FILE_SET CXX_MODULES
        FILES
            "${PREFIX}/src/CLI/Cli.cppm"
            "${PREFIX}/src/Error/Error.cppm"
            "${PREFIX}/src/Server/RPC/RPC.cppm"
            "${PREFIX}/src/Logger/Logger.cppm"
            "${PREFIX}/src/Main/SubMain.cppm"
)
set_target_properties(SUB3
        PROPERTIES
        UNITY_BUILD ON
)