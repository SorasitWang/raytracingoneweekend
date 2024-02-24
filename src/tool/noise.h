#ifndef NOISE_H
#define NOISE_H

#include "../common.h"

class perlin {
public:
    perlin() {
        randVec = new vec3[pointCount];
        for (int i = 0; i < pointCount; ++i) {
            randVec[i] = unit_vector(vec3::random(-1, 1));
        }

        permX = perlin_generate_perm();
        permY = perlin_generate_perm();
        permZ = perlin_generate_perm();
    }

    ~perlin() {
        delete[] randVec;
        delete[] permX;
        delete[] permY;
        delete[] permZ;
    }

    double noise(const point3& p) const {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());

        u = hermite_smoothstep(u);
        v = hermite_smoothstep(v);
        w = hermite_smoothstep(w);

        auto i = static_cast<int>(floor(p.x()));
        auto j = static_cast<int>(floor(p.y()));
        auto k = static_cast<int>(floor(p.z()));
        vec3 c[2][2][2];

        for (int di = 0; di < 2; ++di)
            for (int dj = 0; dj < 2; ++dj)
                for (int dk = 0; dk < 2; ++dk)
                    c[di][dj][dk] = randVec[
                        permX[(i + di) & 255] ^
                            permY[(j + dj) & 255] ^
                            permZ[(k + dk) & 255]
                    ];

        return perlin_interp(c, u, v, w);
    }

    double turb(const point3& p, int depth = 3) const {
        auto accum = 0.0;
        auto tempP = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; ++i) {
            accum += weight * noise(tempP);
            weight *= 0.5;
            tempP *= 2;
        }

        return fabs(accum);
    }

private:
    static const int pointCount = 256;
    vec3* randVec;
    int* permX;
    int* permY;
    int* permZ;

    static int* perlin_generate_perm() {
        auto p = new int[pointCount];

        for (int i = 0; i < perlin::pointCount; ++i)
            p[i] = i;

        permute(p, pointCount);

        return p;
    }

    static void permute(int* p, int n) {
        for (int i = n - 1; i > 0; --i) {
            int target = Util::random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
        auto accum = 0.0;
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                    accum += (i * u + (1 - i) * (1 - u)) *
                            (j * v + (1 - j) * (1 - v)) *
                            (k * w + (1 - k) * (1 - w)) * c[i][j][k];

        return accum;
    }

    static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;

        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k) {
                    vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu))
                        * (j * vv + (1 - j) * (1 - vv))
                        * (k * ww + (1 - k) * (1 - ww))
                        * dot(c[i][j][k], weight_v);
                }

        return accum;
    }

    static float hermite_smoothstep(float x) {
        // x is clamped to 0 ... 1
        return x * x * (3.0 - 2.0 * x);
    }

    static float perline_smoothstep(float x) {
        // x is clamped to 0 ... 1
        return x * x * x * (x * (6.0 * x - 15.0) + 10.0);
    }

};

#endif