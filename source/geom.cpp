#include <vector>
#include <cstddef>
#include "geom_utils.h"
#include "geom.h"

Matrix::Matrix(std::vector<std::vector<float>> matrix) : matrix(std::move(matrix)) {}

Matrix Matrix::operator*(const Matrix &other) const 
{
    size_t rows = matrix.size();
    size_t columns = other.matrix[0].size();        
    size_t indexes = matrix[0].size();

    std::vector<std::vector<float>> new_matrix(rows, std::vector<float>(columns, 0));

    for (size_t row = 0; row < rows; ++row)
    {
        for (size_t index = 0; index < indexes; ++index)
        {
            for (size_t column = 0; column < columns; ++column)
            {
                new_matrix[row][column] += matrix[row][index] * other.matrix[index][column];
            }
        }
    }

    return Matrix(new_matrix);
}

Matrix Point::to_matrix() const
{
    return Matrix({{x, y, z}});
}

static Point from_matrix(const Matrix &other)
{
    return {other.matrix[0][0], other.matrix[0][1], other.matrix[0][2]};
}

Point Point::turned(float x_angle, float y_angle, float z_angle) const
{
    return turned(get_rotation_matrix(x_angle, y_angle, z_angle));
}

Point Point::turned(const Matrix &rotation_matrix) const
{
     return from_matrix(to_matrix() * rotation_matrix);
}

Line::Line(const Point &a, const Point &b) 
{
    this->a = a.y - b.y;
    this->b = b.x - a.x;
    this->c = a.x * b.y - a.y * b.x;
}

float Line::point_position(const Point &point) const
{
    return a * point.x + b * point.y + c;
}
