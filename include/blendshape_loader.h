#ifndef __BLENDSHAPE_LOADER_H__
#define __BLENDSHAPE_LOADER_H__

#include "inttypes.h"

#include <vector>
#include <string>
#include <Eigen/Dense>

typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> MatrixXXf;
typedef Eigen::Matrix<float,Eigen::Dynamic,1> VectorXf;

class BlendshapeMesh;

class BlendshapeLoader
{
public:
    VectorXf* final_shape;
    std::string neutral_shape_path;
    uint32_t get_vertices(float** vertices);
    void update_weights(float* in_weights,uint32_t count);
    MatrixXXf* blendshapes;
    VectorXf* neutral_shape;
    VectorXf* weights;
    std::vector<std::string> expressions;
    uint32_t vertex_count;
public:
    BlendshapeLoader(){}
    ~BlendshapeLoader();
    void load(const char* path);
};

#endif
