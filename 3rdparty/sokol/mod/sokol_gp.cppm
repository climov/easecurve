
module;

#include <sokol_gfx.h>
#include <sokol_gp.h>

export module sokol.gp;

export {
    using ::sgp_error;
    using ::sgp_blend_mode;

    using ::sgp_isize;
    using ::sgp_irect;
    using ::sgp_rect;
    using ::sgp_textured_rect;
    using ::sgp_vec2;
    using ::sgp_point;
    using ::sgp_line;
    using ::sgp_triangle;
    using ::sgp_mat2x3;
    using ::sgp_color;
    using ::sgp_uniform;
    // using ::sgp_images_uniform;
    using ::sgp_state;
    using ::sgp_desc;
    using ::sgp_pipeline_desc;

    using ::sgp_setup;
    using ::sgp_shutdown;
    using ::sgp_is_valid;

    using ::sgp_get_last_error;
    using ::sgp_get_error_message;

    using ::sgp_make_pipeline;

    using ::sgp_begin;
    using ::sgp_flush;
    using ::sgp_end;

    using ::sgp_project;
    using ::sgp_reset_project;

    using ::sgp_push_transform;
    using ::sgp_pop_transform;
    using ::sgp_reset_transform;
    using ::sgp_translate;
    using ::sgp_rotate;
    using ::sgp_rotate_at;
    using ::sgp_scale;
    using ::sgp_scale_at;

    using ::sgp_set_pipeline;
    using ::sgp_reset_pipeline;
    using ::sgp_set_uniform;
    using ::sgp_reset_uniform;

    using ::sgp_set_blend_mode;
    using ::sgp_reset_blend_mode;
    using ::sgp_set_color;
    using ::sgp_reset_color;
    using ::sgp_set_image;
    using ::sgp_unset_image;
    using ::sgp_reset_image;

    using ::sgp_viewport;
    using ::sgp_reset_viewport;
    using ::sgp_scissor;
    using ::sgp_reset_scissor;
    using ::sgp_reset_state;

    using ::sgp_clear;
    using ::sgp_draw_points;
    using ::sgp_draw_point;
    using ::sgp_draw_lines;
    using ::sgp_draw_line;
    using ::sgp_draw_lines_strip;
    using ::sgp_draw_filled_triangles;
    using ::sgp_draw_filled_triangle;
    using ::sgp_draw_filled_triangles_strip;
    using ::sgp_draw_filled_rects;
    using ::sgp_draw_filled_rect;
    using ::sgp_draw_textured_rects;
    using ::sgp_draw_textured_rect;
    // using ::sgp_draw_textured_rects_ex;
    // using ::sgp_draw_textured_rect_ex;

    using ::sgp_query_state;
    using ::sgp_query_desc;
} // export
