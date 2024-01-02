set(IMGUI_PATH imgui)
add_library(imgui STATIC)
target_sources(imgui PRIVATE
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
)
target_sources(imgui INTERFACE FILE_SET HEADERS FILES ${IMGUI_PATH}/imgui.h)
target_sources(imgui PUBLIC FILE_SET CXX_MODULES FILES mod/imgui.cppm)
if (APPLE)
    target_compile_options(imgui PUBLIC -fsized-deallocation)
endif ()
target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_PATH})
