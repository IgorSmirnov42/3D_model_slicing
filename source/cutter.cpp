#include <thread>
#include <iostream>
#include <algorithm>
#include "geom_utils.h"
#include "cutter.h"
#include "ply.h"

class PartitionResult {
public:
    size_t triangles_crossed;
    float x_angle;
    float y_angle;
    float z_angle;
    Point first_point;
    Point last_point;

    PartitionResult(float x_angle, float y_angle, float z_angle) : triangles_crossed(SIZE_MAX),
                                                                       x_angle(x_angle),
                                                                       y_angle(y_angle),
                                                                       z_angle(z_angle),
                                                                       first_point({0, 0, 0}),
                                                                       last_point({2, 3, 9}) {}
};

void build_events(const Figure &figure, std::vector<std::pair<size_t, char>> &events, const Matrix &rotation_matrix)
{
    std::vector<float> turned_points_x;
    turned_points_x.reserve(figure.get_vertices().size());
    for (const Point &vertex : figure.get_vertices())
    {
        turned_points_x.push_back(vertex.turned(rotation_matrix).x);
    }
    events.reserve(figure.get_faces().size() * 2);
    for (const std::vector<size_t> &face : figure.get_faces())
    {
        size_t min_vertex = 0;
        size_t max_vertex = 1;
        if (turned_points_x[face[1]] < turned_points_x[face[min_vertex]])
        {
            min_vertex = 1;
        }
        else {
            max_vertex = 1;
        }
        if (turned_points_x[face[2]] < turned_points_x[face[min_vertex]])
        {
            min_vertex = 2;
        }
        else if (turned_points_x[face[2]] > turned_points_x[face[max_vertex]])
        {
            max_vertex = 2;
        }
        events.emplace_back(face[min_vertex], -1);
        events.emplace_back(face[max_vertex], 1);
    }

    std::sort(events.begin(), events.end(), [&](const std::pair<size_t, char> &a, const std::pair<size_t, char> &b) {
        return turned_points_x[a.first] < turned_points_x[b.first];
    });
}

inline Point rotate(size_t vertex_id, const Figure &figure, const Matrix &rotation_matrix)
{
    return figure.get_vertices()[vertex_id].turned(rotation_matrix);
}

void scanline(const std::vector<std::pair<size_t, char>> & events, const Figure &figure, PartitionResult &result,
        const Matrix &rotation_matrix)
{
    int ctr_intersected = 0;
    size_t ctr_left = 0;
    size_t ctr_right = figure.get_faces().size();
    for (auto &p_e : events)
    {
        size_t point = p_e.first;
        char event_type = p_e.second;
        if (event_type== -1)
        { // open
            --ctr_right;
            ++ctr_intersected;
        }
        else
        { // close
            ++ctr_left;
            --ctr_intersected;
        }

        static const float NORMAL_DIVISION = 1.1;
        if (std::min(ctr_left, ctr_right) * NORMAL_DIVISION >= std::max(ctr_left, ctr_right))
        {
            if (ctr_intersected < result.triangles_crossed)
            {
                result.triangles_crossed = ctr_intersected;
                result.first_point = rotate(point, figure, rotation_matrix);
                result.last_point = result.first_point;
                result.last_point.y += 42; // just a vertical line
            }
        }
        else if (ctr_left > ctr_right)
        {
            break;
        }
    }
}

void find_partition(const Figure &base_figure, PartitionResult &best_result, std::mutex &mutex,
        const Parameters &params)
{
    float x_angle = generate_random_angle();
    float y_angle = generate_random_angle();
    float z_angle = generate_random_angle();

    Matrix rotation_matrix = get_rotation_matrix(x_angle, y_angle, z_angle);

    std::vector<std::pair<size_t, char>> events;
    build_events(base_figure, events, rotation_matrix);

    PartitionResult result = PartitionResult(x_angle, y_angle, z_angle);
    scanline(events, base_figure, result, rotation_matrix);

    mutex.lock();
    if (best_result.triangles_crossed > result.triangles_crossed)
    {
        best_result = result;
    }
    mutex.unlock();
}

void assign_clusters(Figure &figure, Figure &new_figure, const Line &line,
        std::vector<size_t> &left, std::vector<size_t> &right)
{
    static const float DIF = 5;
    std::vector<std::vector<size_t>> new_clusters;

    for (const std::vector<size_t> &cluster : figure.get_clusters())
    {
        size_t left_side = 0;
        size_t right_side = 0;
        for (size_t face_id : cluster)
        {
            const std::vector<size_t> &face = new_figure.get_faces()[face_id];
            Position position = line_triangle_position(line, face, new_figure);
            if (position == LEFT)
            {
                ++left_side;
            }
            else if (position == RIGHT)
            {
                ++right_side;
            }
        }

        // At least DIF:1 division
        if (std::min(left_side, right_side) * DIF < std::max(left_side, right_side))
        {
            for (size_t face_id : cluster)
            {
                (left_side > right_side ? left : right).push_back(face_id);
            }

            new_clusters.push_back(cluster);
        }
    }

    new_figure.set_clusters(new_clusters);
}

// Assigns points that doesn't belong to any cluster
void assign_lone(const Figure &figure, const Line &line,
        std::vector<size_t> &cross, std::vector<size_t> &left, std::vector<size_t> &right)
{
    for (size_t face_id = 0; face_id < figure.get_faces().size(); ++face_id)
    {
        if (figure.get_face2cluster()[face_id] != -1)
        {
            continue;
        }
        const std::vector<size_t> &face = figure.get_faces()[face_id];
        Position position = line_triangle_position(line, face, figure);
        if (position == LEFT)
        {
            left.push_back(face_id);
        }
        else if (position == CROSS)
        {
            cross.push_back(face_id);
        }
        else
        {
            right.push_back(face_id);
        }
    }
}

void assign_cross(std::vector<size_t> &cross, std::vector<size_t> &left, std::vector<size_t> &right)
{
    while (!cross.empty())
    {
        if (left.size() < right.size())
        {
            left.push_back(cross.back());
        }
        else
        {
            right.push_back(cross.back());
        }
        cross.pop_back();
    }
}

std::pair<Figure, Figure> do_partition(const PartitionResult &result, Figure base_figure,
        const std::string &filename, const Parameters &params)
{
    Figure figure = base_figure.turned(result.x_angle, result.y_angle, result.z_angle);
    std::vector<size_t> left;
    std::vector<size_t> right;
    std::vector<size_t> cross;

    left.reserve(figure.get_faces().size() / 2 + result.triangles_crossed);
    right.reserve(figure.get_faces().size() / 2 + result.triangles_crossed);
    cross.reserve(result.triangles_crossed);

    Line line(result.first_point, result.last_point);

    assign_clusters(base_figure, figure, line, left, right);
    assign_lone(figure, line, cross, left, right);
    assign_cross(cross, left, right);

    return { Figure(base_figure, left), Figure(base_figure, right) };
}

void partition(const Figure &figure,
               int depth,
               const std::string &save_filename,
               std::vector<Figure> &division,
               std::mutex &vector_mutex,
               const Parameters &params)
{
    std::cout << save_filename << ' ' << figure.get_faces().size() << std::endl;
    if (depth >= params.depth || figure.get_faces().size() <= params.acceptable_size)
    {
        if (params.save_partition)
        {
            save_figure(figure, save_filename + ".ply");
        }
        vector_mutex.lock();
        division.push_back(figure);
        vector_mutex.unlock();
        return;
    }

    PartitionResult best_result = PartitionResult(-1, -1, -1);

    std::mutex mutex;
    std::vector<std::thread> threads(params.parts - 1);
    for (size_t try_n = 0; try_n < params.parts - 1; ++try_n)
    {
        threads[try_n] = std::thread(find_partition, std::ref(figure), std::ref(best_result), std::ref(mutex),
                std::ref(params));
    }
    find_partition(figure, best_result, mutex, params);
    for (std::thread &thread : threads)
    {
        thread.join();
    }

    std::pair<Figure, Figure> left_right = do_partition(best_result, figure, save_filename, params);

    std::thread left_thread = std::thread(partition, std::ref(left_right.first), depth + 1,
            save_filename + "_l",
            std::ref(division), 
            std::ref(vector_mutex),
            std::ref(params));
    partition(left_right.second, depth + 1, save_filename + "_r",
            division, vector_mutex, params);

    left_thread.join();
}
