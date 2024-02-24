#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../common.h"

class TransformMatrix {

public:

	TransformMatrix() : m() {};

    TransformMatrix(mat4 matrix) : m(matrix) {};

	mat4 mat() { return m; };

	inline TransformMatrix operator*(const TransformMatrix& t2) const  {
		return TransformMatrix( m * t2.m);
	}

    inline point3 operator()(const point3& p) const {
       
        double x = p.x(), y = p.y(), z = p.z();
        double xp = m(0, 0) * x + m(1, 0) * y + m(2, 0) * z + m(3, 0);
        double yp = m(0, 1) * x + m(1, 1) * y + m(2, 1) * z + m(3, 1);
        double zp = m(0, 2) * x + m(1, 2) * y + m(2, 2) * z + m(3, 2);
        double wp = m(0, 3) * x + m(1, 3) * y + m(2, 3) * z + m(3, 3);

        if (wp == 1) return point3(xp, yp, zp);
        else         return point3(xp, yp, zp) / wp;
    }

    inline ray operator()(const ray& r) const {
        point3 o = (*this)(r.origin());
        vec3 d = (*this)(r.direction());
        return ray(o, d, r.time() );
    }

    inline aabb operator()( const aabb& bbox) const {
        point3 max = (*this)(vec3(bbox.x.max, bbox.y.max, bbox.z.max));
        point3 min = (*this)(vec3(bbox.x.min, bbox.y.min, bbox.z.min));

        return aabb(min, max);
    }

    bool inverse(TransformMatrix &out)
    {
        mat4 invOut;
        double inv[16], det;
        int i;

        inv[0] = m[5] * m[10] * m[15] -
            m[5] * m[11] * m[14] -
            m[9] * m[6] * m[15] +
            m[9] * m[7] * m[14] +
            m[13] * m[6] * m[11] -
            m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] +
            m[4] * m[11] * m[14] +
            m[8] * m[6] * m[15] -
            m[8] * m[7] * m[14] -
            m[12] * m[6] * m[11] +
            m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] -
            m[4] * m[11] * m[13] -
            m[8] * m[5] * m[15] +
            m[8] * m[7] * m[13] +
            m[12] * m[5] * m[11] -
            m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] +
            m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] +
            m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] +
            m[1] * m[11] * m[14] +
            m[9] * m[2] * m[15] -
            m[9] * m[3] * m[14] -
            m[13] * m[2] * m[11] +
            m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] -
            m[0] * m[11] * m[14] -
            m[8] * m[2] * m[15] +
            m[8] * m[3] * m[14] +
            m[12] * m[2] * m[11] -
            m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] +
            m[0] * m[11] * m[13] +
            m[8] * m[1] * m[15] -
            m[8] * m[3] * m[13] -
            m[12] * m[1] * m[11] +
            m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] -
            m[0] * m[10] * m[13] -
            m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] -
            m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] -
            m[1] * m[7] * m[14] -
            m[5] * m[2] * m[15] +
            m[5] * m[3] * m[14] +
            m[13] * m[2] * m[7] -
            m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] +
            m[0] * m[7] * m[14] +
            m[4] * m[2] * m[15] -
            m[4] * m[3] * m[14] -
            m[12] * m[2] * m[7] +
            m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] -
            m[0] * m[7] * m[13] -
            m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] -
            m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] +
            m[0] * m[6] * m[13] +
            m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] +
            m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] +
            m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] -
            m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7] +
            m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -
            m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] +
            m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] -
            m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +
            m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] +
            m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -
            m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] -
            m[8] * m[2] * m[5];

        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det == 0)
            return false;

        double invDet = 1.0 / det;

        for (i = 0; i < 16; ++i)
            invOut[i] = inv[i] * invDet;
        out = TransformMatrix(invOut);
        return true;
    }

    void translate(vec3 t) {

        mat4 mat;
        mat(3, 0) = t.x();
        mat(3, 1) = t.y();
        mat(3, 2) = t.z();

        m *= mat;
    }


    void scale(vec3 s) {

        mat4 mat;
        mat(0, 0) = s.x();
        mat(1, 1) = s.y();
        mat(2, 2) = s.z();

        m *= mat;
    }

    void rotate_x(double radian) {

        double cosX = cos(radian), sinX = sin(radian);
        mat4 mat;
        mat(1, 1) = cosX;
        mat(1, 2) = -sinX;
        mat(2, 1) = sinX;
        mat(2, 2) = cosX;

        m *= mat;
    }

    void rotate_y(double radian) {
        double cosX = cos(radian), sinX = sin(radian);
        mat4 mat;
        mat(0, 0) = cosX;
        mat(2, 0) = sinX;
        mat(0, 2) = -sinX;
        mat(2, 2) = cosX;

        m *= mat;
    }

    void rotate_z(double radian) {
        double cosX = cos(radian), sinX = sin(radian);
        mat4 mat;
        m(0, 0) = cosX;
        m(1, 0) = -sinX;
        m(0, 1) = sinX;
        m(1, 1) = cosX;

        m *= mat;
    }

   

private:
	mat4 m;

};

inline std::ostream& operator<<(std::ostream& out, TransformMatrix& t) {
    mat4 m = t.mat();
    return out << m(0, 0) << " " << m(1, 0) << " " << m(2, 0) << " " << m(3, 0) << std::endl
            << m(0, 1) << " " << m(1, 1) << " " << m(2, 1) << " " << m(3, 1) << std::endl
            << m(0, 2) << " " << m(1, 2) << " " << m(2, 2) << " " << m(3, 2) << std::endl
            << m(0, 3) << " " << m(1, 3) << " " << m(2, 3) << " " << m(3, 3) << std::endl;
}

#endif