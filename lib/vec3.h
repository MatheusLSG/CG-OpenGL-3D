#if !defined(_VEC3_H_)
#define _VEC3_H_

#include <cmath>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

class vec3 {
  public:
    GLfloat e[3];

    vec3() : e{0,0,0} {}
    vec3(GLfloat e0, GLfloat e1, GLfloat e2) : e{e0, e1, e2} {}

    GLfloat x() const { return e[0]; }
    GLfloat y() const { return e[1]; }
    GLfloat z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    GLfloat operator[](int i) const { return e[i]; }
    GLfloat& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(GLfloat t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(GLfloat t) {
        return *this *= 1/t;
    }

    GLfloat length() const {
        return std::sqrt(length_squared());
    }

    GLfloat length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
};

using point3 = vec3;
using cor = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(GLfloat t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, GLfloat t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, GLfloat t) {
    return (1/t) * v;
}

inline GLfloat dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

#endif // _VEC3_H_
