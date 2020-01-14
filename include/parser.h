#pragma once

#include <string>
#include <climits>

class Parameters {
public:
    /** Filename to read information from in ply binary format */
    std::string filename = "42.ply";
    /** File to save parameterized result in. By default "uv_`filename`" is used */
    std::string output_filename = "";
    /**
     * Depth for subdivision. If depth is set to N, then mesh will be sliced into 2^N charts
     * (if other division arguments are default) and will be decomposed in UV_atlas
     * using 2^N threads in parallel
     */
    int depth = INT_MAX;
    /**
     * Number of angles that are trying to be used to slice figure each time.
     * The more this number is the more smooth the slicing is.
     * If this number is too big, it may lead to memory overuse.
     */
    size_t parts = 16;
    /**
     * Number of faces in a mesh that is considered to be enough in order not to
     * continue the splicing
     */
    size_t acceptable_size = 0;
    /**
     * If this parameter is turned on, mesh will be separated with considering interesting regions
     * so that every "cluster" of a figure (i.e. tree, small building) will not be sliced.
     * Sometimes it gives better quality for UV-atlas decomposition, but takes a bit more time
     * for preprocessing
     */
    bool clusterization = false;
    /** Max size of mesh region that can be recognized as cluster */
    size_t cluster_max_size = (size_t) 1e5;
    /** Min size of mesh region that can be recognized as cluster */
    size_t cluster_min_size = (size_t) 1e4;
    /** Deletes line-like clusters that may increase quality */
    bool optimize_clusters = false;
    /** If this parameter is on, files with subfigures that are given to parametrizer will be saved */
    bool save_partition = false;
    /** Returns text description of parameters. For instance, can be used as a filename */
    std::string to_string() const;
};

/**
 * Parses command line parameters
 * Format: ./separate_uvatlas <filename> [--depth INT] [--size INT] [--parts INT] [--cluster] [--cluster-min-size INT] [--cluster-max-size INT] [--output STRING] [--part-save]
 * At least one of --depth or --size must be specified
*/
Parameters parse_parameters(int argc, char ** argv);
