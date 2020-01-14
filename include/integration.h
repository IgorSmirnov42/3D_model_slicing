#pragma once

#include "figure.h"
#include "uv_atlas.h"
#include "parametrizedFigure.h"

class Parametrizer {
private:
    geom::UVAtlasGenerator parametrizer;
    void set_division(const std::vector<Figure> &division);
    bool apply();
    ParametrizedFigure get_parametrization_result(const std::vector<Figure> &division);
public:
    ParametrizedFigure parametrize(const std::vector<Figure> &division);
};
