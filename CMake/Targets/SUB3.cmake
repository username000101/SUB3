message(NOTICE "Adding SUB3 executable target")

add_executable(SUB3
        ../../src/Safety/EnableSafetyLinux.cc)

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
        PUBLIC "${PREFIX}/third_party/subprocess/src/cpp"
)

file(GLOB SUB3_src
        "${PREFIX}/main.cpp"
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
target_precompile_headers(SUB3
        PRIVATE "${PREFIX}/src/Utils/Pch.h"
)
set_target_properties(SUB3
        PROPERTIES
        UNITY_BUILD ON
)