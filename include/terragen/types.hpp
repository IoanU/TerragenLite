#pragma once
#include <vector>
#include <cstdint>

namespace terragen {

using Heightmap = std::vector<std::vector<float>>;

enum class Backend {
    FallbackFBM,   // built-in, no deps
    OriginalFBM,   // route to old repo's fbm over Perlin
    DiamondSquare  // (optional) requires original files
};

enum class Sampling {
    Global, // seamless between chunks: sample in world coords
    Local   // replicate old behavior if it normalized by local size
};

} // namespace terragen
