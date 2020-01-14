#pragma once

#include <string>
#include "parser.h"

/**
 * Runs parametrization of mesh located in file with name `filename`
 * Saves uv-parameterized mesh to `save_filename`
 * The quality of division depends on parameters given in `params`
 */
void multi_thread_executor(const std::string& filename,
                           const std::string& save_filename,
                           const Parameters& params);
