#include "geom.h"
#include "figure.h"
#include "geom_utils.h"
#include <vector>
#include <map>
#include <unordered_map>

Figure::Figure(std::vector<Point> vertices,
        std::vector<std::vector<size_t>> faces) : vertices(std::move(vertices)), faces(std::move(faces))
{
    face2cluster = std::vector<int>(this->faces.size(), -1);
}

Figure::Figure(const std::vector<std::array<double, 3>> &points,
        std::vector<std::vector<size_t>> faces) : faces(std::move(faces))
{    
    vertices.reserve(points.size());
    for (const auto &point : points)
    {
        vertices.push_back({(float) point[0], (float) point[1], (float) point[2]});
    }
    face2cluster = std::vector<int>(this->faces.size(), -1);
}

void Figure::set_clusters(const std::vector<std::vector<size_t>> &clusters)
{
    this->clusters = clusters;
    std::fill(face2cluster.begin(), face2cluster.end(), -1);
    for (size_t cluster_id = 0; cluster_id < clusters.size(); ++cluster_id)
    {
        const std::vector<size_t> &cluster = clusters[cluster_id];
        for (size_t face_id : cluster)
        {
            face2cluster[face_id] = cluster_id;
        }
    }
}

std::vector<Point> Figure::turned_points(float x_angle, float y_angle, float z_angle) const
{
    return turned_points(get_rotation_matrix(x_angle, y_angle, z_angle));
}

std::vector<Point> Figure::turned_points(const Matrix &rotation_matrix) const
{
    std::vector<Point> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const Point &point : vertices)
    {
        new_vertices.push_back(point.turned(rotation_matrix));
    }

    return new_vertices;
}

Figure Figure::turned(float x_angle, float y_angle, float z_angle) const
{
    return Figure(turned_points(x_angle, y_angle, z_angle), faces);
}

Figure Figure::turned(const Matrix &rotation_matrix) const
{
    return Figure(turned_points(rotation_matrix), faces);
}

const std::vector<Point> &Figure::get_vertices() const
{
    return vertices;
}

const std::vector<std::vector<size_t>> &Figure::get_faces() const
{
    return faces;
}

const std::vector<std::vector<size_t>> &Figure::get_clusters() const
{
    return clusters;
}

const std::vector<int> &Figure::get_face2cluster() const
{
    return face2cluster;
}

// Used in copy-constructor
template<typename T>
void Figure::copy_vertices(const Figure &figure,
                           const std::vector<size_t> &faces,
                           T &new_index,
                           const std::function<bool(size_t)> &is_presented)
{
    vertices.reserve(figure.vertices.size());
    size_t next_index = 0;
    for (size_t face_id : faces)
    {
        const std::vector<size_t> &face = figure.faces[face_id];
        for (size_t vertex_id : face)
        {
            if (!is_presented(vertex_id))
            {
                vertices.push_back(figure.vertices[vertex_id]);
                new_index[vertex_id] = next_index++;
            }
        }
    }
    vertices.shrink_to_fit();

    this->faces.reserve(faces.size());
    for (size_t face_id : faces)
    {
        const std::vector<size_t> &face = figure.faces[face_id];
        std::vector<size_t> new_face;
        new_face.reserve(face.size());
        for (size_t vertex : face)
        {
            new_face.push_back(new_index[vertex]);
        }
        this->faces.push_back(new_face);
    }
}

Figure::Figure(const Figure &figure, const std::vector<size_t> &faces)
{
    if (faces.size() * 4 >= figure.faces.size())
    {
        // Linear copy
        std::vector<size_t> new_index(figure.vertices.size(), SIZE_MAX);
        copy_vertices(figure, faces, new_index, [&](int idx) {
            return new_index[idx] != SIZE_MAX;
        });
    }
    else
    {
        // Copy using map
        std::map<size_t, size_t> new_index;
        copy_vertices(figure, faces, new_index, [&](int idx) {
            return new_index.count(idx) != 0;
        });
    }

    face2cluster = std::vector<int>(this->faces.size(), -1);

    std::unordered_map<size_t, int> new_cluster_id;
    int cluster = 0;

    for (size_t new_face_id = 0; new_face_id < faces.size(); ++new_face_id)
    {
        size_t old_face_id = faces[new_face_id];
        if (figure.face2cluster[old_face_id] != -1)
        {
            int old_cluster_id = figure.face2cluster[old_face_id];
            if (!new_cluster_id.count(old_cluster_id))
            {
                new_cluster_id[old_cluster_id] = cluster++;
                clusters.emplace_back();
            }
            int cluster_id = new_cluster_id[old_cluster_id];
            clusters[cluster_id].push_back(new_face_id);
            face2cluster[new_face_id] = cluster_id;
        }
    }
}
