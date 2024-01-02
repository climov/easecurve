#pragma once

#if defined(__APPLE__)
#define SOKOL_METAL
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES3
//#define SOKOL_WGPU
#endif
