cmake_minimum_required (VERSION 3.0)

include(GNUInstallDirs)

project(sqlite-test-wal VERSION "0.1" LANGUAGES C)

find_package(PkgConfig)
pkg_check_modules(APP_TEST_DEPS REQUIRED sqlite3)

add_executable(sqlite-test-wal
  src/sqlite-test-wal.c)
set_property(TARGET sqlite-test-wal PROPERTY C_STANDARD 99)
target_compile_options(sqlite-test-wal PUBLIC "-Wall" ${APP_TEST_DEPS_CFLAGS_OTHER})
target_include_directories(sqlite-test-wal PUBLIC ${APP_TEST_DEPS_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)
target_link_libraries(sqlite-test-wal ${APP_TEST_DEPS_LDFLAGS})
install(TARGETS sqlite-test-wal DESTINATION ${CMAKE_INSTALL_BINDIR})
