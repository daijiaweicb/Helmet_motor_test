cmake_minimum_required(VERSION 3.10)
project(GpioProject)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

add_executable(main main.cpp motor.cpp Test.cpp)

target_include_directories(main PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${GPIOD_LIBRARIES})