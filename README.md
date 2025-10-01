# TerragenLite

**Purpose**  
TerragenLite is a tiny, dependency‑free C++ library that generates **deterministic, chunk‑based heightmaps** for voxel games. It is the "integration‑focused" companion to the original **Terragen** repository: use the original repo as your playground and algorithm catalog (Perlin/fBm, Diamond‑Square, Erosion), then plug *the exact same* generation into your game via TerragenLite.

- 🧰 **Zero external deps** (STL only), C++17.
- 🔁 **Deterministic** for identical `(seed, chunk_x, chunk_y, size)`.
- 🔗 **Seamless chunks** via **global coordinate** sampling.
- 🔄 **Dual mode**: built‑in fallback fBm *or* exact algorithms copied from the original repo.
- 🧩 **Drop‑in**: copy `include/ + src/` (or use the single repo) and compile with your engine.

---

## Dependencies

**Mandatory**
- A C++17 compiler (GCC, Clang, MSVC)  
- Standard Library (STL)

**Optional (Original Algorithms Mode)**
- Original Terragen C++ files (e.g., `cpp/perlin.*`, `cpp/fbm.*`, optionally `cpp/diamond_square.*`, `cpp/erosion.*`) copied into `bridges/original/`

> No CMake, no Python required for TerragenLite itself. (You can still use them in your game if you want.)

---

## Relationship to the Original Terragen Repo

TerragenLite is a **lean integration layer**: it exposes a stable C++ API and a clear seam to call into the **exact same algorithms** from the original Terragen repository. The idea is:
1) Explore algorithms, parameters, and previews in the original repo.
2) When you decide what you like, **copy the C++ sources** (Perlin/fBm etc.) into TerragenLite’s `bridges/original/` and compile your game against TerragenLite.
3) You now get **byte‑for‑byte compatible** terrain logic in a minimal, engine‑friendly form.

---

## Repository Layout

```
TerragenLite/
├─ include/terragen/
│  ├─ types.hpp          # aliases & enums (Backend, Sampling)
│  ├─ config.hpp         # parameters & switch between fallback/original
│  └─ terragen.hpp       # public API
├─ src/
│  └─ terragen.cpp       # core impl (fallback + adapter to original)
├─ bridges/
│  └─ original/          # place original Terragen C++ files here
└─ examples/
   └─ main.cpp           # tiny ASCII demo
```

---

## Public API

```cpp
#include <terragen/terragen.hpp>

// Minimal entry (uses preset config)
terragen::Heightmap generate_chunk(int chunk_x, int chunk_y, int size, uint32_t seed);

// Advanced entry with explicit configuration
terragen::Heightmap generate_chunk(int chunk_x, int chunk_y, int size,
                                   uint32_t seed, const terragen::Config& cfg);
```

- `Heightmap` is `std::vector<std::vector<float>>` with values in `[0, 1]`.
- Defaults: **Global** sampling (seamless across chunks), fBm‑style noise.

---

## Quick Start (Fallback Engine)

**Windows PowerShell (single line):**
```powershell
g++ -O2 -std=c++17 examples/main.cpp src/terragen.cpp -I "include" -o demo
.\demo
```

**Linux/macOS (bash):**
```bash
g++ -O2 -std=c++17 examples/main.cpp src/terragen.cpp -Iinclude -o demo
./demo
```

This prints a simple ASCII heightmap preview (2D) for confidence checks.

> PowerShell tip: if you prefer multi‑line commands, use the backtick (`` ` ``) as the line‑continuation character, not a backslash.

---

## Use the **Exact** Original Algorithms

1. Copy these files from the original Terragen repo into `bridges/original/`:
   - `cpp/perlin.h`, `cpp/perlin.cpp`
   - `cpp/fbm.h`, `cpp/fbm.cpp` (optional if you build fBm directly over Perlin)
   - *(optional)* `cpp/diamond_square.*`, `cpp/erosion.*`

2. Edit `include/terragen/config.hpp`:
   ```cpp
   // #define TERRAGENLITE_USE_FALLBACK 1
   #define TERRAGENLITE_USE_FALLBACK 0
   ```

3. Implement the small bridge function (a one‑point sampler) **once**, e.g. in `bridges/original/terragen_bridge.cpp`:
   ```cpp
   #include <algorithm>
   #include <cstdint>
   #include <perlin.h>

   namespace terragen {
   double __terragenlite_original_fbm_point(uint32_t seed,
                                            double x, double y,
                                            int oct, double lac, double gain)
   {
       Perlin per(seed);
       double amp = 1.0, freq = 1.0, sum = 0.0, norm = 0.0;
       for (int i = 0; i < oct; ++i) {
           sum  += amp * per.noise2D(x * freq, y * freq);
           norm += amp;
           amp  *= gain;
           freq *= lac;
       }
       double v = (norm > 0.0) ? (sum / norm) : 0.0; // ~[-1,1]
       v = 0.5 * (v + 1.0);                           // -> [0,1]
       return std::clamp(v, 0.0, 1.0);
   }
   } // namespace terragen
   ```

4. Build including the original sources + bridge.

**Windows PowerShell (single line):**
```powershell
g++ -O2 -std=c++17 examples/main.cpp src/terragen.cpp `
  bridges/original/perlin.cpp bridges/original/fbm.cpp `
  bridges/original/terragen_bridge.cpp `
  -I "include" -I "bridges/original" -o demo_original
.\demo_original
```

**Linux/macOS (bash):**
```bash
g++ -O2 -std=c++17 examples/main.cpp src/terragen.cpp     bridges/original/perlin.cpp bridges/original/fbm.cpp     bridges/original/terragen_bridge.cpp     -Iinclude -Ibridges/original -o demo_original
./demo_original
```

---

## Step‑by‑Step Integration into a Voxel Engine

1. **Add files to your project**
   - Copy `include/terragen/` and `src/terragen.cpp` into your engine’s source tree (or keep as a submodule/repo).
   - Optionally copy originals into `bridges/original/` and set `TERRAGENLITE_USE_FALLBACK 0` to match the playground outputs exactly.

2. **Include the API**
   ```cpp
   #include <terragen/terragen.hpp>
   ```

3. **Generate a heightmap for a chunk**
   ```cpp
   auto hm = terragen::generate_chunk(chunk_x, chunk_y, size, world_seed);
   ```

4. **Place voxels based on height**
   ```cpp
   int maxH = 128; // your world column height
   for (int y = 0; y < size; ++y) {
     for (int x = 0; x < size; ++x) {
       int H = int(hm[y][x] * (maxH - 1));   // 0..maxH-1
       for (int z = 0; z <= H; ++z) placeBlock(x, y, z, Stone);
       placeBlock(x, y, H, Grass);
     }
   }
   ```

5. **Ensure seamless meshes at chunk borders**
   - Use a **half‑open grid** (do not duplicate the right/top edge in each chunk mesh), or
   - Generate `(size+1)×(size+1)` samples and discard the last row/column when storing, or
   - Add small “skirts” on chunk edges.

6. **Tune terrain feel**
   - Increase `cfg.fbm.scale` → larger continents (smoother).
   - Increase `cfg.fbm.octaves` → more detail.
   - Decrease `cfg.fbm.gain` → smoother surfaces.
   - Optional shaping: `h = pow(h, 1.15f)` for steeper mountains.

7. **Multithreading (optional)**
   - Generate chunks in worker threads: the API is pure and stateless per call.

---

## Configuration Notes

```cpp
terragen::Config cfg = terragen::preset_oldrepo_like();
cfg.sampling = terragen::Sampling::Global; // default (seamless)
cfg.fbm.scale = 64.0;     // feature size
cfg.fbm.octaves = 6;      // detail
cfg.fbm.lacunarity = 2.0; // frequency growth
cfg.fbm.gain = 0.5;       // amplitude falloff

auto hm = terragen::generate_chunk(cx, cy, size, seed, cfg);
```

- `Sampling::Global` samples world coordinates (`wx = cx*size + x`), ensuring **no seams**.
- `Sampling::Local` can reproduce “local‑normalized” looks if you specifically want A/B visual parity with some older snapshots.
