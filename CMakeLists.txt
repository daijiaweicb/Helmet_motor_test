cmake_minimum_required(VERSION 3.10)
project(GpioProject)

# 查找 gpiod 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

add_executable(main main.cpp)

# 链接 gpiod
target_include_directories(main PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${GPIOD_LIBRARIES})