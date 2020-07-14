#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef _WIN32

#include <filesystem>

namespace fs = std::filesystem;

#else

#ifndef GHC_FILESYSTEM_IMPLEMENTATION
#include "ghc/fs_fwd.hpp"
#endif

namespace fs = ghc::filesystem;

#endif

#endif // FILESYSTEM_H
