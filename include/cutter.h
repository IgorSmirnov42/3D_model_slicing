#pragma once

#include <cmath>
#include <mutex>
#include <random>
#include "figure.h"
#include "parser.h"

/**
 * Cuts `figure` according to given parameters in `params`
 * All the resulting subfigures will be stored in `division`
 * `vector_mutex` is mutex for `division`
 * `save_filename` is name for saving subfigures if `params.save_partition` is true
 * `save_filename` has suffix like "_l_r_r" that shows that base figure was divided 3 times
 *      and this part was by left side in first partition, and by right side in second and third
 * `depth` shows how many partitions were done before with this figure. On start should be 0
 */
void partition(const Figure &figure,
               int depth,
               const std::string &save_filename,
               std::vector<Figure> &division,
               std::mutex &vector_mutex,
               const Parameters &params);
