cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cpppistache_chat VERSION 1.0.0 LANGUAGES CXX)

add_executable(chat_server)
target_compile_features(chat_server PRIVATE cxx_std_17)
target_sources(chat_server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp")
target_include_directories(chat_server PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/entity"
    "${CMAKE_CURRENT_SOURCE_DIR}/dep/include"
)
find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)
target_link_libraries(chat_server PRIVATE PkgConfig::Pistache)

add_subdirectory(dep/ftxui)

add_executable(chat_client)
target_compile_features(chat_client PRIVATE cxx_std_17)
target_sources(chat_client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp")
target_include_directories(chat_client PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/entity"
    "${CMAKE_CURRENT_SOURCE_DIR}/dep/include"
)
target_link_libraries(chat_client PRIVATE ftxui::screen ftxui::dom ftxui::component PkgConfig::Pistache)



