#pragma once 

#include "figure.h"
#include "geom.h"
#include "geom.h"
#include <vector>

enum Position {
    LEFT,
    RIGHT,
    CROSS
};

Position line_triangle_position(const Line &line, const std::vector<size_t> &face, const Figure &figure);

Matrix get_rotation_matrix(float x_angle, float y_angle, float z_angle);

/** Returns random number in [0; 2*PI] */
float generate_random_angle();

class Vector3d {
public:
    float x, y, z;

    void normalize();
};

Vector3d operator+(const Vector3d &a, const Vector3d &b);

Vector3d operator*(float x, const Vector3d &a);

Vector3d operator/(const Vector3d &a, float x);

Vector3d build_normal(size_t face_id, const Figure& figure);

float distance(Vector3d a, Vector3d b);

