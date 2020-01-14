#include <iostream>
#include <utility>
#include "parametrizedFigure.h"

const std::vector<Point2d> &ParametrizedFigure::get_uvs()
{
    return uvs;
}

const std::vector<std::vector<size_t>> &ParametrizedFigure::get_uvfaces() {
    return uvfaces;
}

ParametrizedFigure::ParametrizedFigure(const std::vector<Point> &vertices,
        const std::vector<std::vector<size_t>> &faces,
        std::vector<Point2d> uvs,
        std::vector<std::vector<size_t>> uvfaces) : Figure(vertices, faces),
                                                    uvs(std::move(uvs)),
                                                    uvfaces(std::move(uvfaces)) {}
