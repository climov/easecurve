
module;

#include <sokol_app.h>

export module sokol.app;

export {
    using ::SAPP_MAX_TOUCHPOINTS;
    using ::SAPP_MAX_MOUSEBUTTONS;
    using ::SAPP_MAX_KEYCODES;
    using ::SAPP_MAX_ICONIMAGES;

    using ::sapp_event_type;
    using ::sapp_keycode;
    using ::sapp_android_tooltype;
    using ::sapp_touchpoint;
    using ::sapp_mousebutton;

    using ::SAPP_MODIFIER_SHIFT;
    using ::SAPP_MODIFIER_CTRL;
    using ::SAPP_MODIFIER_ALT;
    using ::SAPP_MODIFIER_SUPER;
    using ::SAPP_MODIFIER_LMB;
    using ::SAPP_MODIFIER_RMB;
    using ::SAPP_MODIFIER_MMB;

    using ::sapp_event;
    using ::sapp_range;
    using ::sapp_image_desc;
    using ::sapp_icon_desc;
    using ::sapp_allocator;
    using ::sapp_log_item;
    using ::sapp_logger;
    using ::sapp_desc;
    using ::sapp_html5_fetch_error;
    using ::sapp_html5_fetch_response;
    using ::sapp_html5_fetch_request;

    using ::sapp_mouse_cursor;

    using ::sapp_isvalid;
    using ::sapp_width;
    using ::sapp_widthf;
    using ::sapp_height;
    using ::sapp_heightf;
    using ::sapp_color_format;
    using ::sapp_depth_format;
    using ::sapp_sample_count;
    using ::sapp_high_dpi;
    using ::sapp_dpi_scale;
    using ::sapp_show_keyboard;
    using ::sapp_keyboard_shown;
    using ::sapp_is_fullscreen;
    using ::sapp_toggle_fullscreen;
    using ::sapp_show_mouse;
    using ::sapp_mouse_shown;
    using ::sapp_lock_mouse;
    using ::sapp_mouse_locked;
    using ::sapp_set_mouse_cursor;
    using ::sapp_get_mouse_cursor;
    using ::sapp_userdata;
    using ::sapp_query_desc;
    using ::sapp_request_quit;
    using ::sapp_cancel_quit;
    using ::sapp_quit;
    using ::sapp_consume_event;
    using ::sapp_frame_count;
    using ::sapp_frame_duration;
    using ::sapp_set_clipboard_string;
    using ::sapp_get_clipboard_string;
    using ::sapp_set_window_title;
    using ::sapp_set_icon;
    using ::sapp_get_num_dropped_files;
    using ::sapp_get_dropped_file_path;

    using ::sapp_run;

    using ::sapp_egl_get_display;
    using ::sapp_egl_get_context;

    using ::sapp_html5_ask_leave_site;
    using ::sapp_html5_get_dropped_file_size;
    using ::sapp_html5_fetch_dropped_file;

    using ::sapp_metal_get_device;
    using ::sapp_metal_get_renderpass_descriptor;
    using ::sapp_metal_get_drawable;
    using ::sapp_macos_get_window;
    using ::sapp_ios_get_window;

    using ::sapp_d3d11_get_device;
    using ::sapp_d3d11_get_device_context;
    using ::sapp_d3d11_get_swap_chain;
    using ::sapp_d3d11_get_render_target_view;
    using ::sapp_d3d11_get_depth_stencil_view;
    using ::sapp_win32_get_hwnd;

    using ::sapp_wgpu_get_device;
    using ::sapp_wgpu_get_render_view;
    using ::sapp_wgpu_get_resolve_view;
    using ::sapp_wgpu_get_depth_stencil_view;

    using ::sapp_android_get_native_activity;
} // export
