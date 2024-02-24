#ifndef IMAGE_H
#define IMAGE_H

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#pragma warning (disable: 4996)
#endif

#define STBI_FAILURE_USERMSG

#include <cstdlib>
#include <iostream>

#include "../util.h"
#include "../../lib/stb_image.h"

enum class InterpolateMethod { Nearest, Linear };

enum class WrapMethod { Repeat, MirroedRepeat, ClampToEdge, ClampToBorder };

class Image {
public:
    Image(InterpolateMethod _interpolateMethod = InterpolateMethod::Nearest) : data(nullptr), 
        colorScale( 1.0 / 255.0), interpolateMethod(_interpolateMethod) {}

    Image(const char* imageFilepath, InterpolateMethod _interpolateMethod = InterpolateMethod::Nearest): 
        colorScale(1.0 / 255.0), interpolateMethod(_interpolateMethod) {
        // Loads image data from the specified file. If the RTW_IMAGES environment variable is
        // defined, looks only in that directory for the image file. If the image was not found,
        // searches for the specified image file first from the current directory, then in the
        // images/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
        // parent, on so on, for six levels up. If the image was not loaded successfully,
        // width() and height() will return 0.

        auto filename = std::string(imageFilepath);
        auto imagedir = "res";

        // Hunt for the image file in some likely locations.
        if (imagedir && load(std::string(imagedir) + "/" + imageFilepath)) return;

        std::cerr << "ERROR: Could not load image file '" << std::string(imagedir) + "/" + imageFilepath << "'.\n";

    }

    ~Image() { 
        stbi_image_free(data);
    }

    bool load(const std::string filename, int numChannel = 3) {
        bytesPerPixel = numChannel;
        // Loads image data from the given file name. Returns true if the load succeeded.
        auto n = bytesPerPixel; // Dummy out parameter: original components per pixel
        data = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &n, bytesPerPixel);
        bytesPerScanline = imageWidth * bytesPerPixel;
        return data != nullptr;
    }

    int width()  const { return (data == nullptr) ? 0 : imageWidth; }
    int height() const { return (data == nullptr) ? 0 : imageHeight; }

    double *normalizeColor(unsigned char* color) const {
        double colorNorm[3];
        
        colorNorm[0] = color[0] * colorScale;
        colorNorm[1] = color[1] * colorScale;
        colorNorm[2] = color[2] * colorScale;

        return colorNorm;
    }
    const double* pixel_color(double u, double v) const {
        // Return the address of the $bytesPerPixel of the pixel at x,y (or magenta if no data).
        
        // Flip Y
        v = 1 - v;

        if (data == nullptr) return noDataColor;
        
        if (u < 0 || u > 1 || v < 0 || v > 1) {
            // std::cout << u << " " << v << std::endl;
            if (wrapMethod == WrapMethod::ClampToEdge) {
                u = Util::clamp(u, 0.0, 1.0);
                v = Util::clamp(v, 0.0, 1.0);
            }
            else if (wrapMethod == WrapMethod::Repeat) {
                u = u - static_cast<int>(u);
                v = v - static_cast<int>(v);
            }
            else if (wrapMethod == WrapMethod::MirroedRepeat) {
                u = 1 - (u - static_cast<int>(u));
                v = 1 - (v - static_cast<int>(v));
            }
            else if (wrapMethod == WrapMethod::ClampToBorder) {
                return noDataColor;
            }
        }

        if (interpolateMethod == InterpolateMethod::Nearest) {
            int x = static_cast<int>(u * width());
            int y = static_cast<int>(v * height());

            x = Util::clamp(x, 0, imageWidth - 1);
            y = Util::clamp(y, 0, imageHeight - 1);

            unsigned char* color = data + y * bytesPerScanline + x * bytesPerPixel;
           
            return normalizeColor(color);
        }
        else if (interpolateMethod == InterpolateMethod::Linear) {
            double x = u * width();
            double y = v * height();
           
            unsigned int xInt = static_cast<int>(x), yInt = static_cast<int>(y);

            //  At the border, just return like a Nearest
            if ( xInt == 0 || yInt == 0 
                || xInt == imageWidth - 1 || yInt == imageHeight - 1)
                return normalizeColor(data + yInt * bytesPerScanline + xInt * bytesPerPixel);

            int neighDir[2];
            neighDir[0] = x - xInt < 0.5 ? 1 : -1;
            neighDir[1] = y - yInt < 0.5 ? 1 : -1;

            unsigned char* color00, * color01, * color10, * color11;
            color00 = data + yInt * bytesPerScanline + xInt * bytesPerPixel;
            color01 = data + yInt * bytesPerScanline + (xInt + neighDir[0]) * bytesPerPixel;
            color10 = data + (yInt + neighDir[1]) * bytesPerScanline + xInt * bytesPerPixel;
            color11 = data + (yInt + neighDir[1]) * bytesPerScanline + (xInt + neighDir[0]) * bytesPerPixel;
           
            double dist[2];
            if (neighDir[0] == 1)
                dist[0] = x - xInt - 0.5;
            else
                dist[0] = 0.5 - (x - xInt);

            if (neighDir[1] == 1)
                dist[1] = y - yInt - 0.5;
            else
                dist[1] = 0.5 - (y - yInt);

            double pow2Dist[2] = { dist[0] * dist[0], dist[1] * dist[1] };
            //  pow( 1-x, 2 )
            double pow2OneMinusDist[2] = { 1 - 2 * dist[0] + pow2Dist[0], 1 - 2 * dist[1] + pow2Dist[1] };

            double w00, w01, w10, w11;
            w00 = sqrt(pow2Dist[0]          + pow2Dist[1]);
            w01 = sqrt(pow2OneMinusDist[0]  + pow2Dist[1]);
            w10 = sqrt(pow2Dist[0]          + pow2OneMinusDist[1]);
            w11 = sqrt(pow2OneMinusDist[0]  + pow2OneMinusDist[1]);

            double wTotal = 1 / (w00 + w01 + w10 + w11);
            double multiplier = colorScale * wTotal;
            double color[3];
            color[0] = (w00 * color00[0] + w01 * color01[0] + w10 * color10[0] + w11 * color11[0]) * multiplier;
            color[1] = (w00 * color00[1] + w01 * color01[1] + w10 * color10[1] + w11 * color11[1]) * multiplier;
            color[2] = (w00 * color00[2] + w01 * color01[2] + w10 * color10[2] + w11 * color11[2]) * multiplier;
 
            return color ;
        }
    }

private:
    int bytesPerPixel;
    unsigned char* data;
    int imageWidth, imageHeight;
    int bytesPerScanline;
    double colorScale;

    static constexpr double noDataColor[3] = { 1.0, 0.0, 1.0 };

    InterpolateMethod interpolateMethod = InterpolateMethod::Nearest;
    WrapMethod wrapMethod = WrapMethod::ClampToEdge;

};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#pragma warning (enable: 4996)
#endif

#endif