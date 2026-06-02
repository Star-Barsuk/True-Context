set(
    TRUECONTEXT_CORE_SOURCES
    io/stdout_writer.cpp
    io/file_writer.cpp
    util/path.cpp
    cli/parser.cpp
    cli/usage.cpp
    app/application.cpp
    walk/path_list.cpp
    walk/walker.cpp
    ignore/matcher.cpp
    sort/sorter.cpp
    render/renderer.cpp
    render/tree_renderer.cpp
    collect/collector.cpp
)

add_library(truecontext_core STATIC ${TRUECONTEXT_CORE_SOURCES})
configure_target(truecontext_core)

target_include_directories(
    truecontext_core
    PUBLIC
    "${PROJECT_PUBLIC_INCLUDE_DIR}"
)

add_executable(true_context main.cpp)
configure_target(true_context)

target_link_libraries(
    true_context
    PRIVATE
    truecontext_core
)
