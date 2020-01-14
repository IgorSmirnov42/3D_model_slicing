#pragma once

#include <vector>

class Matrix {
public:
    std::vector<std::vector<float>> matrix;
    explicit Matrix(std::vector<std::vector<float>> matrix);
    Matrix operator*(const Matrix &other) const;
};

class Point {
public:
    float x, y, z;

    Matrix to_matrix() const;
    Point turned(float x_angle, float y_angle, float z_angle) const;
    Point turned(const Matrix &rotation_matrix) const;
};

class Point2d {
public:
    float x, y;
};

class Line {
public:
    float a, b, c;

    Line(const Point &a, const Point &b);
    float point_position(const Point &point) const;
};
