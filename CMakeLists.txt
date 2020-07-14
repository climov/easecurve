cmake_minimum_required(VERSION 3.5)

project(easecurve LANGUAGES C CXX)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FMT_PATH 3rdparty/fmt-6.2.1)
set(FMT_SOURCES ${FMT_PATH}/src/format.cc ${FMT_PATH}/src/os.cc)

set(FS_PATH 3rdparty/filesystem-1.3.2)

set(SFML_PATH 3rdparty/SFML-2.5.1)
if(WIN32)
    if(CMAKE_CL_64)
        set(SFML_LIB_PATH "${SFML_PATH}/win64")
    else()
        set(SFML_LIB_PATH "${SFML_PATH}/win32")
    endif()
else()
    set(SFML_LIB_PATH "${SFML_PATH}/lib")
endif()

set(IMGUI_PATH 3rdparty/imgui-1.76)
set(IMGUI_SOURCES ${IMGUI_PATH}/imgui.cpp ${IMGUI_PATH}/imgui_widgets.cpp ${IMGUI_PATH}/imgui_draw.cpp ${IMGUI_PATH}/imgui_demo.cpp)

set(IMGUI_SFML_PATH 3rdparty/imgui-sfml-2.1)
set(IMGUI_SFML_SOURCES ${IMGUI_SFML_PATH}/imgui-SFML.cpp)

set(APP_SOURCES
    main.cpp
    src/filesystem.cpp
    src/trace.cpp
    src/render.cpp
    src/easecurve.cpp
)

add_executable(easecurve ${FMT_SOURCES} ${IMGUI_SOURCES} ${IMGUI_SFML_SOURCES} ${APP_SOURCES})

if(WIN32)
    string(JOIN " " SRC_COMPILE_FLAGS
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
    )
else()
    string(JOIN " " SRC_COMPILE_FLAGS
        -Wall -Wextra -Weverything -Werror
        -Wno-c++98-compat -Wno-c++98-compat-pedantic
        -Wno-documentation-unknown-command
        -Wno-padded
        -Wno-exit-time-destructors -Wno-global-constructors
        -Wno-shadow-uncaptured-local
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-switch-enum
    )
endif()

set_source_files_properties(${APP_SOURCES} PROPERTIES COMPILE_FLAGS ${SRC_COMPILE_FLAGS})

target_include_directories(easecurve PRIVATE "src")
target_include_directories(easecurve SYSTEM PRIVATE "${SFML_PATH}/include" "${FS_PATH}/include" "${FMT_PATH}/include" "${IMGUI_PATH}" "${IMGUI_SFML_PATH}")

target_link_directories(easecurve PRIVATE "${SFML_LIB_PATH}")
if(WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(easecurve PRIVATE sfml-graphics-d sfml-window-d sfml-system-d sfml-audio-d ${OPENGL_LIBRARIES})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(easecurve PRIVATE sfml-graphics sfml-window sfml-system sfml-audio ${OPENGL_LIBRARIES} c++)
endif()
