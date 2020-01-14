cmake_minimum_required (VERSION 3.5)
project (separate_uvatlas)

include_directories(. include)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (OpenMP REQUIRED)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

link_directories( ${CMAKE_SOURCE_DIR})

set (SOURCES
        source/main.cpp
        source/geom.cpp
        source/figure.cpp
        source/parser.cpp
        source/geom_utils.cpp
        source/integration.cpp
        source/ply.cpp
        source/cutter.cpp
        source/interesting.cpp
        source/executor.cpp
        source/parameterizedFigure.cpp)

set (HEADERS
        uv_atlas.h
        geom.h
        figure.h
        parser.h
        geom_utils.h
        ply.h
        integration.h
        cutter.h
        interesting.h
        include/executor.h
        include/parametrizedFigure.h)

add_executable(${PROJECT_NAME} 
                       source/main.cpp 
                       source/geom.cpp 
                       source/figure.cpp 
                       source/parser.cpp 
                       source/geom_utils.cpp 
                       source/integration.cpp
                       source/ply.cpp
                       source/cutter.cpp
                       source/interesting.cpp
                       source/executor.cpp
                       source/parameterizedFigure.cpp)



target_link_libraries(${PROJECT_NAME} mesh uvatlas ${OpenMP_LIBS})
