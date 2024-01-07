cmake_minimum_required(VERSION 3.28)

project(easecurve LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)

add_subdirectory(3rdparty)

if (WIN32)
    set(SRC_COMPILE_FLAGS
        -Wall -WX
        -experimental:external -external:W0 -external:anglebrackets
        -wd4514
        -wd4820
        -wd5045
        -wd4625 -wd4626 -wd5026 -wd5027
        -wd4244
        -wd4061
        -wd4710 -wd4711
        -wd4738
        -wd4582 -wd4365
        -wd4623
        -wd5219
        -wd4267
        -wd4686
    )
else ()
    set(SRC_COMPILE_FLAGS
        -fsized-deallocation
        -Wall -Wextra -Weverything -Werror
        -Wno-c++98-compat -Wno-c++98-compat-pedantic
        -Wno-documentation-unknown-command
        -Wno-padded
        -Wno-exit-time-destructors -Wno-global-constructors
        -Wno-shadow-uncaptured-local
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-switch-enum
        -Wno-implicit-int-float-conversion
        -Wno-missing-prototypes
    )
endif ()

add_executable(easecurve)
target_sources(easecurve PRIVATE main.cpp src/render.cpp)
target_sources(easecurve PRIVATE FILE_SET CXX_MODULES FILES src/easecurve.cppm src/appstate.cppm)
target_compile_options(easecurve PRIVATE ${SRC_COMPILE_FLAGS})
target_include_directories(easecurve PRIVATE "src")
target_link_libraries(easecurve PRIVATE alx sokol)

if (WIN32)
    target_link_options(easecurve PRIVATE -subsystem:WINDOWS)
elseif (APPLE)
    target_link_libraries(easecurve PRIVATE "-framework Cocoa" "-framework QuartzCore" "-framework Metal" "-framework MetalKit")
elseif (EMSCRIPTEN)
    target_link_options(easecurve PRIVATE -s USE_WEBGL2=1)
    add_custom_command(OUTPUT easecurve.html DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/easecurve.html COMMENT "Copying html" COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/easecurve.html" "easecurve.html")
    add_custom_target(html ALL DEPENDS easecurve.html)
endif()
