#pragma once

#include "geom.h"
#include <vector>
#include <array>
#include <functional>

/**
 * Class for storing model in a program
 */
class Figure {
private:
    std::vector<Point> vertices;
    std::vector<std::vector<size_t>> faces;
    /**
     * Sets of faces that belongs to one component of a figure
     * If it is possible, after partition every face of each cluster will be in same subfigure
     * However, partition may split cluster if division is inconsistent
     */
    std::vector<std::vector<size_t>> clusters;
    /**
     * For each face number of cluster it belongs
     * If face doesn't belong to any cluster, -1
     */
    std::vector<int> face2cluster;
    template<typename T>
    void copy_vertices(const Figure &figure, const std::vector<size_t> &faces, T &new_index,
            const std::function<bool(size_t)> &is_presented);

public:
    const std::vector<Point> &get_vertices() const;
    const std::vector<std::vector<size_t>> &get_faces() const;
    const std::vector<std::vector<size_t>> &get_clusters() const;
    const std::vector<int> &get_face2cluster() const;
    /** Creates figure without clusters */
    Figure(std::vector<Point> vertices, std::vector<std::vector<size_t>> faces);
    /** Sets new clusters. If any clusters were set before, they will be removed */
    void set_clusters(const std::vector<std::vector<size_t>> &clusters);
    /** Creates figure without clusters with vertices created from `points` */
    Figure(const std::vector<std::array<double, 3>> &points, std::vector<std::vector<size_t>> faces);
    /** Returns vertices of figure turned on given angles */
    std::vector<Point> turned_points(float x_angle, float y_angle, float z_angle) const;
    /** Returns vertices of figure turned on given matrix */
    std::vector<Point> turned_points(const Matrix &rotation_matrix) const;
    /**
     * Returns figure turned on given angles.
     * Attention! Does not save clusters
     */
    Figure turned(float x_angle, float y_angle, float z_angle) const;
    /**
     * Returns figure turned on given matrix.
     * Attention! Does not save clusters
     */
    Figure turned(const Matrix &rotation_matrix) const;
    /** Creates new figure as subfigure of `figure`, taking from `figure` only faces from `faces` */
    Figure(const Figure &figure, const std::vector<size_t> &faces);
};
