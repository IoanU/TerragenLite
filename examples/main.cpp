#include <iostream>
#include <terragen/terragen.hpp>

int main(){
    const int chunk_x = 0, chunk_y = 0, size = 32;
    const uint32_t seed = 1337u;

    // Default config: fallback FBM, global sampling (seamless)
    auto hm = terragen::generate_chunk(chunk_x, chunk_y, size, seed);

    // Print ASCII map
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            std::cout << (hm[y][x] > 0.5f ? '#' : '.');
        }
        std::cout << '\n';
    }
    return 0;
}
