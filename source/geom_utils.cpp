#include <cmath>
#include <set>
#include <random>
#include <mutex>
#include "geom_utils.h"

Position line_triangle_position(const Line &line, const std::vector<size_t> &face, const Figure &figure)
{
    static const float INTERSECT_EPS = 1e-6;
    std::set<int> signs;
    for (size_t point_id : face)
    {
        float point_position = line.point_position(figure.get_vertices()[point_id]);
        if (std::fabs(point_position) > INTERSECT_EPS)
        {
            signs.insert(point_position < 0 ? -1 : 1);
        }
        else 
        {
            return CROSS;
        }
    }
    if (signs.size() != 1)
    {
        return CROSS;
    }
    return *signs.begin() == 1 ? LEFT : RIGHT;
}

Matrix get_rotation_matrix(float x_angle, float y_angle, float z_angle)
{
    Matrix x_matrix = Matrix({{1, 0,            0},
                              {0, std::cos(x_angle), -std::sin(x_angle)},
                              {0, std::sin(x_angle), std::cos(x_angle)}});

    Matrix y_matrix = Matrix({{std::cos(y_angle),  0, std::sin(y_angle)},
                              {0,             1, 0},
                              {-std::sin(y_angle), 0, std::cos(y_angle)}});

    Matrix z_matrix = Matrix({{std::cos(z_angle), -std::sin(z_angle), 0},
                              {std::sin(z_angle), std::cos(z_angle),  0},
                              {0,            0,             1}});

    return x_matrix * y_matrix * z_matrix;
}

float generate_random_angle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::mutex mutex;
    static const double PI = atan2(0, -1);
    static std::uniform_real_distribution<> urd(0, 2 * PI);

    mutex.lock();
    float angle = urd(gen);
    mutex.unlock();

    return angle;
}

void Vector3d::normalize()
{
    float len = std::sqrt(x * x + y * y + z * z);
    if (fabs(len) > 1e-7)
    {
        x /= len;
        y /= len;
        z /= len;
    }
}

Vector3d operator+(const Vector3d &a, const Vector3d &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3d operator*(float x, const Vector3d &a)
{
    return {a.x * x, a.y * x, a.z * x};
}

Vector3d operator/(const Vector3d &a, float x)
{
    return {a.x / x, a.y / x, a.z / x};
}

Vector3d build_normal(size_t face_id, const Figure& figure)
{
    const Point &p1 = figure.get_vertices()[figure.get_faces()[face_id][0]];
    const Point &p2 = figure.get_vertices()[figure.get_faces()[face_id][1]];
    const Point &p3 = figure.get_vertices()[figure.get_faces()[face_id][2]];
    float x = (p2.y - p1.y) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.y - p1.y);
    float y = (p2.z - p1.z) * (p3.x - p1.x) - (p2.x - p1.x) * (p3.z - p1.z);
    float z = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    Vector3d normal = {x, y, z};
    normal.normalize();
    return normal;
}

float distance(Vector3d a, Vector3d b)
{
    a.normalize();
    b.normalize();
    return (float) sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}
