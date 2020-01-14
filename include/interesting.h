#pragma once

#include "figure.h"
#include "parser.h"
#include <vector>

/** Finds interesting regions of mesh (clusters) with size specified in parameters */
std::vector<std::vector<size_t>> divide_interesting(const Figure &figure, const Parameters &params);
