
module;

#include <sokol_gfx.h>

export module sokol.gfx;

export {
    using ::sg_buffer;
    using ::sg_image;
    using ::sg_shader;
    using ::sg_pipeline;
    using ::sg_pass;
    using ::sg_context;
    using ::sg_range;

    using ::SG_INVALID_ID;
    using ::SG_NUM_SHADER_STAGES;
    using ::SG_NUM_INFLIGHT_FRAMES;
    using ::SG_MAX_COLOR_ATTACHMENTS;
    using ::SG_MAX_SHADERSTAGE_BUFFERS;
    using ::SG_MAX_SHADERSTAGE_IMAGES;
    using ::SG_MAX_SHADERSTAGE_UBS;
    using ::SG_MAX_UB_MEMBERS;
    using ::SG_MAX_VERTEX_ATTRIBUTES;
    using ::SG_MAX_MIPMAPS;
    using ::SG_MAX_TEXTUREARRAY_LAYERS;

    using ::sg_color;

    using ::sg_backend;
    using ::sg_pixel_format;

    using ::sg_pixelformat_info;
    using ::sg_features;
    using ::sg_limits;

    using ::sg_resource_state;
    using ::sg_usage;
    using ::sg_buffer_type;
    using ::sg_index_type;
    using ::sg_image_type;
    using ::sg_sampler_type;
    using ::sg_cube_face;
    using ::sg_shader_stage;
    using ::sg_primitive_type;
    using ::sg_filter;
    using ::sg_wrap;
    using ::sg_border_color;
    using ::sg_vertex_format;
    using ::sg_vertex_step;
    using ::sg_uniform_type;
    using ::sg_uniform_layout;
    using ::sg_cull_mode;
    using ::sg_face_winding;
    using ::sg_compare_func;
    using ::sg_stencil_op;
    using ::sg_blend_factor;
    using ::sg_blend_op;
    using ::sg_color_mask;
    using ::sg_action;

    using ::sg_color_attachment_action;
    using ::sg_depth_attachment_action;
    using ::sg_stencil_attachment_action;
    using ::sg_pass_action;
    using ::sg_bindings;
    using ::sg_buffer_desc;
    using ::sg_image_data;
    using ::sg_image_desc;
    using ::sg_shader_attr_desc;
    using ::sg_shader_uniform_desc;
    using ::sg_shader_uniform_block_desc;
    using ::sg_shader_image_desc;
    using ::sg_shader_stage_desc;
    using ::sg_shader_desc;
    using ::sg_buffer_layout_desc;
    using ::sg_vertex_attr_desc;
    using ::sg_layout_desc;
    using ::sg_stencil_face_state;
    using ::sg_stencil_state;
    using ::sg_depth_state;
    using ::sg_blend_state;
    using ::sg_color_state;
    using ::sg_pipeline_desc;
    using ::sg_pass_attachment_desc;
    using ::sg_pass_desc;
    using ::sg_trace_hooks;
    using ::sg_slot_info;
    using ::sg_buffer_info;
    using ::sg_image_info;
    using ::sg_shader_info;
    using ::sg_pipeline_info;
    using ::sg_pass_info;

    using ::sg_log_item;

    using ::sg_metal_context_desc;
    using ::sg_d3d11_context_desc;
    using ::sg_wgpu_context_desc;
    using ::sg_context_desc;
    using ::sg_commit_listener;
    using ::sg_allocator;
    using ::sg_logger;
    using ::sg_desc;

    using ::sg_setup;
    using ::sg_shutdown;
    using ::sg_isvalid;
    using ::sg_reset_state_cache;
    using ::sg_install_trace_hooks;
    using ::sg_push_debug_group;
    using ::sg_pop_debug_group;
    using ::sg_add_commit_listener;
    using ::sg_remove_commit_listener;

    using ::sg_make_buffer;
    using ::sg_make_image;
    using ::sg_make_shader;
    using ::sg_make_pipeline;
    using ::sg_make_pass;
    using ::sg_destroy_buffer;
    using ::sg_destroy_image;
    using ::sg_destroy_shader;
    using ::sg_destroy_pipeline;
    using ::sg_destroy_pass;
    using ::sg_update_buffer;
    using ::sg_update_image;
    using ::sg_append_buffer;
    using ::sg_query_buffer_overflow;
    using ::sg_query_buffer_will_overflow;

    using ::sg_begin_default_pass;
    using ::sg_begin_default_passf;
    using ::sg_begin_pass;
    using ::sg_apply_viewport;
    using ::sg_apply_viewportf;
    using ::sg_apply_scissor_rect;
    using ::sg_apply_scissor_rectf;
    using ::sg_apply_pipeline;
    using ::sg_apply_bindings;
    using ::sg_apply_uniforms;
    using ::sg_draw;
    using ::sg_end_pass;
    using ::sg_commit;

    using ::sg_query_desc;
    using ::sg_query_backend;
    using ::sg_query_features;
    using ::sg_query_limits;
    using ::sg_query_pixelformat;

    using ::sg_query_buffer_state;
    using ::sg_query_image_state;
    using ::sg_query_shader_state;
    using ::sg_query_pipeline_state;
    using ::sg_query_pass_state;

    using ::sg_query_buffer_info;
    using ::sg_query_image_info;
    using ::sg_query_shader_info;
    using ::sg_query_pipeline_info;
    using ::sg_query_pass_info;

    using ::sg_query_buffer_desc;
    using ::sg_query_image_desc;
    using ::sg_query_shader_desc;
    using ::sg_query_pipeline_desc;
    using ::sg_query_pass_desc;

    using ::sg_query_buffer_defaults;
    using ::sg_query_image_defaults;
    using ::sg_query_shader_defaults;
    using ::sg_query_pipeline_defaults;
    using ::sg_query_pass_defaults;

    using ::sg_alloc_buffer;
    using ::sg_alloc_image;
    using ::sg_alloc_shader;
    using ::sg_alloc_pipeline;
    using ::sg_alloc_pass;
    using ::sg_dealloc_buffer;
    using ::sg_dealloc_image;
    using ::sg_dealloc_shader;
    using ::sg_dealloc_pipeline;
    using ::sg_dealloc_pass;
    using ::sg_init_buffer;
    using ::sg_init_image;
    using ::sg_init_shader;
    using ::sg_init_pipeline;
    using ::sg_init_pass;
    using ::sg_uninit_buffer;
    using ::sg_uninit_image;
    using ::sg_uninit_shader;
    using ::sg_uninit_pipeline;
    using ::sg_uninit_pass;
    using ::sg_fail_buffer;
    using ::sg_fail_image;
    using ::sg_fail_shader;
    using ::sg_fail_pipeline;
    using ::sg_fail_pass;

    using ::sg_setup_context;
    using ::sg_activate_context;
    using ::sg_discard_context;

    using ::sg_d3d11_device;

    using ::sg_mtl_device;
    using ::sg_mtl_render_command_encoder;
} // export
