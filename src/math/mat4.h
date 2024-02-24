#ifndef MAT4_H
#define MAT4_H

#include <stdexcept>

class mat4 {

public:
    
    mat4() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = (i == j) ? 1 : 0;
    }
    
    mat4(const double mat[4][4]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = mat[i][j];
    }

    mat4 operator+(const mat4& m) const {
        mat4 r = *this;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] += m.m[i][j];
        return r;
    }

    
    mat4 operator*(double s) const {
        mat4 r = *this;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] *= s;
        return r;
    }

    mat4 operator/(double s) const {
        
        if (s == 0)
            throw std::runtime_error( "Divide by zero" );
        mat4 r = *this;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] /= s;
        return r;
    }

    
    bool operator==(const mat4& m2) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != m2.m[i][j])
                    return false;
        return true;
    }

    bool operator!=(const mat4& m2) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != m2.m[i][j])
                    return true;
        return false;
    }

    
    bool operator<(const mat4& m2) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                if (m[i][j] < m2.m[i][j])
                    return true;
                if (m[i][j] > m2.m[i][j])
                    return false;
            }
        return false;
    }

    mat4& operator*=(const mat4& m2)
    {
        mat4 temp;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    temp.m[i][j] += (m[i][k] * m2.m[k][j]);
                }
            }
        }
        return (*this = temp);
    }

    mat4& operator*=(double num)
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] *= num;
            }
        }
        return *this;
    }
   
    double& operator()(int x, int y) { return m[x][y]; }
    double operator()(int x, int y) const { return m[x][y]; }

    double& operator[](const int idx) {
        unsigned int row = idx / 4;
        unsigned int col = idx - (row * 4);
        return m[row][col];

    }

private:
    double m[4][4];
};

inline mat4 operator*(const mat4& m1, const mat4& m2)
{

    mat4 temp(m1);
    return (temp *= m2);
}


#endif