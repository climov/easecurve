set(SOKOL_PATH sokol)
set(SOKOL_GP_PATH sokol_gp)

add_library(sokol STATIC)
target_sources(sokol PRIVATE
    mod/sokol_color.cpp
    mod/sokol_imgui.cpp
    mod/sokol_glue.c
    mod/sokol_log.c
)
if(APPLE)
    target_sources(sokol PRIVATE mod/sokol_gp_mac.m mod/sokol_app_mac.m)
else()
    target_sources(sokol PRIVATE mod/sokol_gp.c mod/sokol_app.c)
endif()

target_sources(sokol PRIVATE FILE_SET HEADERS FILES
    ${SOKOL_PATH}/sokol_app.h
    ${SOKOL_PATH}/sokol_args.h
    ${SOKOL_PATH}/sokol_audio.h
    ${SOKOL_PATH}/sokol_fetch.h
    ${SOKOL_PATH}/sokol_gfx.h
    ${SOKOL_PATH}/sokol_glue.h
    ${SOKOL_PATH}/sokol_log.h
    ${SOKOL_PATH}/sokol_time.h
    ${SOKOL_PATH}/util/sokol_color.h
    ${SOKOL_PATH}/util/sokol_debugtext.h
    ${SOKOL_PATH}/util/sokol_fontstash.h
    ${SOKOL_PATH}/util/sokol_gfx_imgui.h
    ${SOKOL_PATH}/util/sokol_gl.h
    ${SOKOL_PATH}/util/sokol_imgui.h
    ${SOKOL_PATH}/util/sokol_memtrack.h
    ${SOKOL_PATH}/util/sokol_nuklear.h
    ${SOKOL_PATH}/util/sokol_shape.h
    ${SOKOL_PATH}/util/sokol_spine.h
    ${SOKOL_GP_PATH}/sokol_gp.h
)
target_sources(sokol PUBLIC FILE_SET CXX_MODULES FILES
    mod/sokol.cppm
    mod/sokol_app.cppm
    mod/sokol_color.cppm
    mod/sokol_gfx.cppm
    mod/sokol_glue.cppm
    mod/sokol_gp.cppm
    mod/sokol_imgui.cppm
    mod/sokol_log.cppm
)
if (APPLE)
    target_compile_options(sokol PUBLIC -fsized-deallocation)
elseif (LINUX)
    target_compile_options(sokol PUBLIC -fsized-deallocation)
elseif (WIN32)
    target_compile_options(sokol PUBLIC -utf-8)
endif ()
target_include_directories(sokol SYSTEM PUBLIC ${SOKOL_PATH} ${SOKOL_GP_PATH})
target_link_libraries(sokol PUBLIC imgui)
