#include "parser.h"
#include <iostream>

Parameters parse_parameters(int argc, char ** argv)
{
    if (argc < 4)
    {
        std::cerr << "Wrong input format! Filename and at least one of --depth or --size must be specified." << std::endl;
        abort();
    }
    Parameters params = Parameters();
    params.filename = std::string(argv[1]);
    for (int i = 2; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--depth")
        {
            if (i == argc - 1)
            {
                std::cerr << "INT expected after --depth." << std::endl;
                abort();
            }
            params.depth = atoi(argv[i + 1]);
            if (params.depth < 0)
            {
                std::cerr << "--depth cannot be negative." << std::endl;
                abort();
            }
            ++i;
        }
        else if (std::string(argv[i]) == "--size")
        {
            if (i == argc - 1)
            {
                std::cerr << "INT expected after --size." << std::endl;
                abort();
            }
            params.acceptable_size = atoi(argv[i + 1]);
            ++i;
        }
        else if (std::string(argv[i]) == "--parts")
        {
            if (i == argc - 1)
            {
                std::cerr << "INT expected after --parts." << std::endl;
                abort();
            }
            params.parts = atoi(argv[i + 1]);
            ++i;
        }
        else if (std::string(argv[i]) == "--cluster") 
        {
            params.clusterization = true;
        }
        else if (std::string(argv[i]) == "--cluster-opt")
        {
            params.optimize_clusters = true;
        }
        else if (std::string(argv[i]) == "--cluster-min-size")
        {
            if (i == argc - 1)
            {
                std::cerr << "INT expected after --cluster-min-size." << std::endl;
                abort();
            }
            params.cluster_min_size = atoi(argv[i + 1]);
            ++i;
        }
        else if (std::string(argv[i]) == "--cluster-max-size")
        {
            if (i == argc - 1)
            {
                std::cerr << "INT expected after --cluster-max-size." << std::endl;
                abort();
            }
            params.cluster_max_size = atoi(argv[i + 1]);
            ++i;
        }
        else if (std::string(argv[i]) == "--output")
        {
            if (i == argc - 1) {
                std::cerr << "STRING expected after --output." << std::endl;
                abort();
            }
            params.output_filename = argv[i + 1];
            ++i;
        }
        else if (std::string(argv[i]) == "--part-save")
        {
            params.save_partition = true;
        }
        else
        {
            std::cerr << "Unexpected token " << argv[i] << std::endl;
            abort();
        }
    }
    if (params.output_filename.empty())
    {
        params.output_filename = "uv_" + params.filename;
    }
    if (params.cluster_min_size > params.cluster_max_size)
    {
        std::cerr << "Wrong arguments! Min size of cluster cannot be more than max." << std::endl;
        abort();
    }
    if (params.cluster_max_size > params.acceptable_size && params.depth == INT_MAX)
    {
        std::cerr << "Wrong arguments! Max size of cluster cannot be more than acceptable size of cluster without --depth specified." << std::endl;
        abort();
    }
    if (params.depth == INT_MAX && params.acceptable_size == 0)
    {
        std::cerr << "At least one of --depth or --size must be specified." << std::endl;
        abort();
    }
    if (params.depth < 0)
    {
        std::cerr << "--depth must be non-negative!" << std::endl;
        abort();
    }
    return params;
}

std::string Parameters::to_string() const
{
    return filename + "_" + std::to_string(depth) + "_" + 
        std::to_string(acceptable_size) + "_" + 
        std::to_string(parts) + "_" + 
        std::to_string(clusterization) + "_" + 
        std::to_string(cluster_min_size) + "_" + 
        std::to_string(cluster_max_size) + "_" +
        std::to_string(optimize_clusters);
}
