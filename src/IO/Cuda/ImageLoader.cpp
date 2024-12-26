#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ImageLoader.h"

namespace dyno {

    bool ImageLoader::loadImage(const char* path, CArray2D<Vec4f>& img) {
        int x, y, comp;
        stbi_set_flip_vertically_on_load(true);

        float* data = stbi_loadf(path, &x, &y, &comp, STBI_default);

        if (data) {
            img.resize(x, y);
            for (int x0 = 0; x0 < x; x0++) {
                for (int y0 = 0; y0 < y; y0++) {
                    int idx = (y0 * x + x0) * comp;
                    for (int c0 = 0; c0 < comp; c0++) {
                        img(x0, y0)[c0] = data[idx + c0];
                    }
                }
            }
            STBI_FREE(data);
        }

        return data != nullptr;
    }

} // namespace dyno