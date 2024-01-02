
module;

#include <sokol_gfx.h>
#include <sokol_app.h>
#include <util/sokol_imgui.h>

export module sokol.imgui;

export {
    using ::simgui_allocator_t;
    using ::simgui_desc_t;
    using ::simgui_frame_desc_t;

    using ::simgui_setup;
    using ::simgui_new_frame;
    using ::simgui_render;
#if !defined(SOKOL_IMGUI_NO_SOKOL_APP)
    using ::simgui_handle_event;
    using ::simgui_map_keycode;
#endif
    using ::simgui_shutdown;
} // export
