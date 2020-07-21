#ifndef __BLENDSHAPE_MODIFIER_H__
#define __BLENDSHAPE_MODIFIER_H__

#include <Eigen/Dense>
#include <vector>

typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> MatrixXXf;

class BlendshapeLoader;

class Modifier
{
    /// Solving
    /// k = (mu + alpha)I
    /// (B^t * B + k)^-1 * (B^t*(m-m0) + alpha*(w^t-1))
public:
    float mu,alpha,beta;
    MatrixXXf* B;
    MatrixXXf* w;
    MatrixXXf* wo;
    MatrixXXf* wd;
    MatrixXXf* m0;
    MatrixXXf* m;

    int rows,cols;
    int active_index;
    std::vector<int> existing_vertex;

    Modifier(BlendshapeLoader& loader);
    ~Modifier();
};

class BlendshapeModifier
{
public:
    Modifier modifier;
    BlendshapeModifier(BlendshapeLoader& loader);
    ~BlendshapeModifier();
    void add_vertex(BlendshapeLoader& loader,int index,const float* weights);
    void calculate_weights(const float* in_vertex,const float* weights,bool use_old=false);
    void end_drag();
    void render_ui(const float* weights);
};

#endif