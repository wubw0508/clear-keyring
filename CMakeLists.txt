cmake_minimum_required(VERSION 3.13)
project(dpa-ext-gnomekeyring-demo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

find_package(PkgConfig REQUIRED)

# 查找必要的库
pkg_check_modules(libsecret REQUIRED IMPORTED_TARGET libsecret-unstable)
add_compile_definitions(SECRET_API_SUBJECT_TO_CHANGE)

pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(gio REQUIRED IMPORTED_TARGET gio-2.0)

# 添加可执行文件
add_executable(demo_keyring demo_keyring.cpp)

# 包含目录
target_include_directories(demo_keyring PRIVATE
    /usr/include
)

# 链接库
target_link_libraries(demo_keyring PRIVATE
    PkgConfig::libsecret
    PkgConfig::glib
    PkgConfig::gio
)

# 安装目标
install(TARGETS demo_keyring DESTINATION bin)