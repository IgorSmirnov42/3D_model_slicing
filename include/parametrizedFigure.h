#pragma once

#include "figure.h"

class ParametrizedFigure : public Figure {
private:
    /**
     * For each face indices of its uv coordinates in `uvs` vector
     * `uvfaces` must have same order with `faces`
     */
    std::vector<std::vector<size_t>> uvfaces;
    std::vector<Point2d> uvs;
public:
    const std::vector<Point2d> &get_uvs();
    const std::vector<std::vector<size_t>> &get_uvfaces();
    ParametrizedFigure(const std::vector<Point> &vertices, const std::vector<std::vector<size_t>> &faces,
            std::vector<Point2d> uvs, std::vector<std::vector<size_t>> uvfaces);
};
