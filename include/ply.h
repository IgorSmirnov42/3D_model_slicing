#pragma once 

#include "happly.h"
#include "figure.h"
#include "uv_atlas.h"
#include "integration.h"
#include <vector>
#include <string>

/** Reads mesh from file without UVs */
Figure read_mesh(const std::string &path_to_ply);

/** Saves figure after decomposition into UV-atlas */
void save_figure(ParametrizedFigure &parameterized, const std::string &filename);

/** Saves figure without UVs */
void save_figure(const Figure &figure, const std::string &filename);
