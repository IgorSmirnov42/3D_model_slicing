#include <chrono>
#include "happly.h"
#include "figure.h"
#include "parser.h"
#include "ply.h"
#include "integration.h"
#include "cutter.h"
#include "interesting.h"
#include <vector>
#include <mutex>
#include <fstream>

long get_current_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void multi_thread_executor(const std::string& filename, const std::string& save_filename, const Parameters& params)
{
    long start_time = get_current_time();
    Figure figure = read_mesh(filename);
    std::cout << "Read mesh " << filename << std::endl;

    long read_mesh_time = get_current_time();
    if (params.clusterization)
    {
        figure.set_clusters(divide_interesting(figure, params));
        std::cout << "Found " << figure.get_clusters().size() << " clusters" << std::endl;
        for (const auto &cluster : figure.get_clusters())
        {
            std::cout << "Cluster size " << cluster.size() << std::endl;
        }
    }
    long cluster_time = get_current_time();
    if (params.clusterization)
    {
        std::cout << "Clusterization done in " << (float) (cluster_time - read_mesh_time) / 1000 << std::endl;
    }

    std::vector<Figure> division;
    std::mutex mutex;
    partition(figure, 0, save_filename, division, mutex, params);

    long partition_time = get_current_time();
    std::cout << "Partition done in " << (float) (partition_time - cluster_time) / 1000 << std::endl;

    Parametrizer parametrizer;
    ParametrizedFigure result = parametrizer.parametrize(division);

    save_figure(result, params.output_filename);

    long now = get_current_time();
    float time_calc = (float) (now - start_time) / 1000;

    std::cout << "Calculated in " << time_calc << " seconds" << std::endl;
}
