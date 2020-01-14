#include "figure.h"
#include "interesting.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include "geom_utils.h"

class Graph {
public:
    std::vector<std::vector<size_t>> edges;
    std::vector<bool> used;
    std::vector<size_t> color;
    int n = 0;

    void clear_used()
    {
        std::fill(used.begin(), used.end(), false);
    }

    void resize(std::size_t n)
    {
        this->n = n;
        edges.resize(n);
        used.resize(n, false);
        color.resize(n, 0);
    }

    void interesting_bfs(size_t vertex_id, size_t color, const std::vector<Vector3d> &normals,
            std::vector<size_t> &this_color);
};

std::pair<size_t, size_t> &sort_pair(std::pair<size_t, size_t> &pair)
{
    if (pair.first > pair.second)
    {
        std::swap(pair.first, pair.second);
    }
    return pair;
}

Graph figure2graph(const Figure &figure)
{
    std::map<std::pair<size_t, size_t>, std::vector<size_t>> neighbours;
    for (size_t face_id = 0; face_id < figure.get_faces().size(); ++face_id)
    {
        const std::vector<size_t> &face = figure.get_faces()[face_id];
        for (size_t first_vertex = 0; first_vertex < face.size(); ++first_vertex)
        {
            for (size_t second_vertex = first_vertex + 1; second_vertex < face.size(); ++second_vertex)
            {
                std::pair<size_t, size_t> edge = {face[first_vertex], face[second_vertex]};
                neighbours[sort_pair(edge)].push_back(face_id);
            }
        }
    }

    Graph graph = Graph();
    graph.resize(figure.get_faces().size());

    for (const auto &edge_neighbours : neighbours)
    {
        const std::vector<size_t> &current_neighbours = edge_neighbours.second;
        for (size_t face_a : current_neighbours)
        {
            for (size_t face_b : current_neighbours)
            {
                if (face_a != face_b)
                {
                    graph.edges[face_a].push_back(face_b);
                }
            }
        }
    }
    return graph;
}

float max_distance(const std::vector<size_t> &this_color, const std::vector<Vector3d> &normals, const Vector3d &norm)
{
    float max_dist = 0;
    for (size_t id : this_color)
    {
        float new_dist = distance(normals[id], norm);
        max_dist = std::max(max_dist, new_dist);
    }
    return max_dist;
}


/* Finds component for `vertex_id` vertex (face).
 * Binary relation "to be in one component" is not transitive
 */
void Graph::interesting_bfs(size_t vertex_id, size_t current_color, const std::vector<Vector3d> &normals,
        std::vector<size_t> &this_color)
{
    static const float DIFF = 0.4;
    std::queue<size_t> q;
    q.push(vertex_id);
    used[vertex_id] = true;
    Vector3d mean = {0, 0, 0}; // mean normal vector in a component
    while (!q.empty())
    {
        vertex_id = q.front();
        mean = (this_color.size() * mean + normals[vertex_id]) / ((float) this_color.size() + 1);
        q.pop();
        this_color.push_back(vertex_id);
        color[vertex_id] = current_color;
        for (size_t neighbour : edges[vertex_id])
        {
            if (!used[neighbour] && distance(mean, normals[neighbour]) < DIFF)
            {
                q.push(neighbour);
                used[neighbour] = true;
            }
        }
    }
}

bool is_not_line_kind(const std::vector<size_t> &small_figure, const Figure &base_figure) {
    Figure unturned = Figure(base_figure, small_figure);
    for (size_t i = 0; i < 20; ++i)
    {
        float x_angle = generate_random_angle();
        float y_angle = generate_random_angle();
        float z_angle = generate_random_angle();

        std::vector<Point> turned_points = unturned.turned_points(x_angle, y_angle, z_angle);

        const float INF = 1e9;
        float min_x = INF;
        float max_x = -INF;
        float min_y = INF;
        float max_y = -INF;
        float min_z = INF;
        float max_z = -INF;

        for (const Point &p : turned_points)
        {
            min_x = std::min(min_x, p.x);
            max_x = std::max(max_x, p.x);
            min_y = std::min(min_y, p.y);
            max_y = std::max(max_y, p.y);
            min_z = std::min(min_z, p.z);
            max_z = std::max(max_z, p.z);
        }

        float len1 = max_x - min_x;
        float len2 = max_y - min_y;
        float len3 = max_z - min_z;
        float max_len = std::max({len1, len2, len3});

        if (max_len > 3 * (len1 + len2 + len3 - max_len))
        {
            return false;
        }
    }
    return true;
}

/*
 * Connects some clusters to build bigger cluster from them
 */
std::vector<std::vector<size_t>> do_small_connection(std::vector<std::vector<size_t>> figures, const Figure &figure, 
    const Parameters &params)
{
    std::map<size_t, std::set<size_t>> vertex2figures; // which figures from `figures` have this vertex
    for (size_t figure_id = 0; figure_id < figures.size(); ++figure_id)
    {
        const std::vector<size_t> &face_ids = figures[figure_id];
        for (size_t face_id : face_ids)
        {
            for (size_t vertex_id : figure.get_faces()[face_id])
            {
                vertex2figures[vertex_id].insert(figure_id);
           }
        }
    }

    std::set<std::pair<size_t, size_t>> ordered_figures;
    for (size_t i = 0; i < figures.size(); ++i)
    {
        ordered_figures.insert({figures[i].size(), i});
    } 
    std::vector<std::vector<size_t>> ans;

    while (!ordered_figures.empty())
    {
        // Take figure with least number of neighbours
        size_t cur_figure_id = ordered_figures.begin()->second;
        ordered_figures.erase(ordered_figures.begin());
        
        size_t best_result = 0;
        size_t best_id = 0;
        std::map<size_t, size_t> neighbours;
        // Choose figure that is adjacent with current by maximum number of vertices
        // and such that combining it with current, we eill have not a very big cluster
        for (size_t face_id : figures[cur_figure_id])
        {
            for (size_t vertex_id : figure.get_faces()[face_id])
            {
                for (size_t figure_id : vertex2figures[vertex_id])
                {
                    if (figures[figure_id].size() + figures[cur_figure_id].size() > params.cluster_max_size || 
                        figure_id == cur_figure_id)
                    {
                        continue;
                    }
                    size_t res = ++neighbours[figure_id];
                    if (res > best_result)
                    {
                        best_result = res;
                        best_id = figure_id;
                    }
                }
            }
        }

        if (best_result == 0)
        {
            if (!params.optimize_clusters || is_not_line_kind(figures[cur_figure_id], figure)) {
                ans.push_back(figures[cur_figure_id]);
            }
        }
        else
        {
            for (size_t face_id : figures[cur_figure_id])
            {
                for (size_t vertex_id : figure.get_faces()[face_id])
                {
                    vertex2figures[vertex_id].erase(cur_figure_id);
                    vertex2figures[vertex_id].insert(best_id);
                }
            }

            ordered_figures.erase({figures[best_id].size(), best_id});
            while (!figures[cur_figure_id].empty())
            {
                figures[best_id].push_back(figures[cur_figure_id].back());
                figures[cur_figure_id].pop_back();
            }
            ordered_figures.insert({figures[best_id].size(), best_id});
        }
    }
    return ans;
}

std::vector<std::vector<size_t>> run_bfs(Graph &graph, const Figure &figure, const Parameters &params)
{
    std::vector<Vector3d> normals;
    normals.reserve(figure.get_vertices().size());
    for (size_t face_id = 0; face_id < figure.get_faces().size(); ++face_id)
    {
        normals.push_back(build_normal(face_id, figure));
    }
    size_t color = 0;
    std::vector<std::vector<size_t>> small_figures;
    for (size_t vertex_id = 0; vertex_id < graph.n; ++vertex_id)
    {
        if (!graph.used[vertex_id])
        {
            std::vector<size_t> faces;
            graph.interesting_bfs(vertex_id, color++, normals, faces);
            if (faces.size() <= params.cluster_max_size)
            {
                if (!params.optimize_clusters || faces.size() < 1000 || is_not_line_kind(faces, figure)) {
                    small_figures.push_back(faces);
                }
            }
        }
    }
    std::vector<std::vector<size_t>> figures = do_small_connection(small_figures, figure, params);
    std::vector<std::vector<size_t>> ans;
    ans.reserve(figures.size());
    for (const std::vector<size_t> &small_figure : figures)
    {
        if (small_figure.size() >= params.cluster_min_size)
        {
            ans.push_back(small_figure);
        }
    }
    ans.shrink_to_fit();
    return ans;
}

std::vector<std::vector<size_t>> divide_interesting(const Figure &figure, const Parameters &params)
{
    Graph graph = figure2graph(figure);
    return run_bfs(graph, figure, params);
}
