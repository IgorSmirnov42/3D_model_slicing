#include "happly.h"
#include "figure.h"
#include "ply.h"
#include <vector>
#include <string>

Figure read_mesh(const std::string &path_to_ply)
{
    happly::PLYData plyIn(path_to_ply);
    return Figure(plyIn.getVertexPositions(), plyIn.getFaceIndices<size_t>());
}

void save_figure(const Figure& figure, const std::string& filename)
{
    std::vector<std::array<double, 3>> vertices;
    vertices.reserve(figure.get_vertices().size());
    for (const Point& point : figure.get_vertices())
    {
        vertices.push_back({point.x, point.y, point.z});
    }
    happly::PLYData plyOut;

    plyOut.addVertexPositions(vertices);
    std::vector<std::vector<size_t>> faces = figure.get_faces();
    plyOut.addFaceIndices(faces);
    plyOut.write(filename, happly::DataFormat::Binary);
}

void save_figure(ParametrizedFigure &figure, const std::string &filename) {
    std::vector<std::array<double, 3>> vertices;
    vertices.reserve(figure.get_vertices().size());
    for (const Point& point : figure.get_vertices())
    {
        vertices.push_back({point.x, point.y, point.z});
    }
    happly::PLYData plyOut;

    plyOut.addVertexPositions(vertices);
    std::vector<std::vector<size_t>> faces = figure.get_faces();
    plyOut.addFaceIndices(faces);

    std::vector<std::vector<float>> texcoords;
    texcoords.reserve(figure.get_uvfaces().size());

    for (const auto &face : figure.get_uvfaces())
    {
        std::vector<float> new_texcoords;
        new_texcoords.reserve(6);
        for (int v_id = 0; v_id < 3; ++v_id)
        {
            const size_t &vertex = face[v_id];
            new_texcoords.push_back(figure.get_uvs()[vertex].x);
            new_texcoords.push_back(figure.get_uvs()[vertex].y);
        }
        texcoords.push_back(new_texcoords);
    }
    plyOut.getElement("face").addListProperty("texcoord", texcoords);
    plyOut.write(filename, happly::DataFormat::Binary);
}
