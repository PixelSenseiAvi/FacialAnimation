#include "blendshape_modifier.h"
#include "blendshape_loader.h"

#include <iostream>
#include "ui/imgui.h"

Modifier::Modifier(BlendshapeLoader& loader)
: rows(0),
active_index(-1)
{
    int vertex_count = loader.blendshapes->rows();
    int blendshape_count = loader.blendshapes->cols();

    cols = blendshape_count;

    B = new MatrixXXf(vertex_count,blendshape_count);
    w = new MatrixXXf(blendshape_count,1);
    wo = new MatrixXXf(blendshape_count,1);
    wd = new MatrixXXf(blendshape_count,1);
    
    w->setZero();
    wo->setZero();
    wd->setZero();

    m0 = new MatrixXXf(vertex_count,1);
    m = new MatrixXXf(vertex_count,1);

    mu = 0.001f;
    alpha = 0.3f;
    beta = 0.6f;

}

Modifier::~Modifier()
{
    if(B)
        delete B;
    if(w)
        delete w;
    if(wo)
        delete wo;
    if(wd)
        delete wd;
    if(m0)
        delete m0;
    if(m)
        delete m;
}

BlendshapeModifier::BlendshapeModifier(BlendshapeLoader& loader):modifier(loader)
{    
}

BlendshapeModifier::~BlendshapeModifier()
{
    
}

void BlendshapeModifier::add_vertex(BlendshapeLoader& loader,int index,const float* weights)
{
    std::cout << "index : " << index << std::endl;
    for(int i=0;i<modifier.existing_vertex.size();i++)
    {
        if(modifier.existing_vertex[i] == index)
        {
            //std::cout << "vertex already exists" << std::endl;
            modifier.active_index = i * 3;
            return;
        }
    }

    int matrix_index = modifier.rows;
    modifier.rows += 3;

    modifier.existing_vertex.push_back(index);
    modifier.active_index = matrix_index;

    int row_index = index * 3;
    
    int rows = modifier.rows;
    int cols = modifier.cols;

    modifier.B->conservativeResize(rows,cols);
    (modifier.B)->block(matrix_index,0,3,cols) = (loader.blendshapes)->block(row_index,0,3,cols);

    modifier.m->conservativeResize(rows,1);

    modifier.m0->conservativeResize(rows,1);
    (modifier.m0)->block(matrix_index,0,3,1) = (loader.neutral_shape)->block(row_index,0,3,1);

    std::cout << "Vertices under pin: " << modifier.m->rows()/3 << std::endl;
}

void BlendshapeModifier::calculate_weights(const float* in_vertex,const float* weights,bool use_old)
{
    /// Solving
    /// k = (mu + alpha)I
    /// (B^t * B + k)^-1 * (B^t*(m-m0) + alpha*(w^t-1))
    if(!use_old)
    {
        (*modifier.m)(modifier.active_index + 0,0) = in_vertex[0];
        (*modifier.m)(modifier.active_index + 1,0) = in_vertex[1];
        (*modifier.m)(modifier.active_index + 2,0) = in_vertex[2];
    }

    /// Fix from Seo et al.
    for(int i=0;i<24;i++)
    {
        (*modifier.wo)(i,0) = weights[i];
    }


    MatrixXXf delta_m = ((*modifier.m) - (*modifier.m0));
    int total_rows = modifier.m->rows() + 72;
    MatrixXXf K(modifier.cols,modifier.cols);
    K.setIdentity();
    MatrixXXf A(total_rows,modifier.cols);
    A << *modifier.B, modifier.alpha * K , modifier.mu * K, modifier.beta * K;

    MatrixXXf B(total_rows,1);
    B << delta_m, modifier.alpha* (*modifier.wo), modifier.beta * (*modifier.wd), 0.0f * (*modifier.w);

    /// QR Decomposition
    /// Slower but better accuracy
    MatrixXXf w = A.colPivHouseholderQr().solve(B);

    for(int i=0;i<w.rows();i++)
    {
        if(w(i,0) < 0)
            w(i,0) = 0;

        if(w(i,0) > 1)
            w(i,0) = 1;
    }

    /// LDLT
    //MatrixXXf w = A.ldlt().solve(B);
    *modifier.wd = w;
    *modifier.w = w;

    // std::cout << "final weights : " << *modifier.w << std::endl;
}

void BlendshapeModifier::end_drag()
{

}

void BlendshapeModifier::render_ui(const float* weights)
{
    ImGui::Begin("Modifier Slider");
    ImGui::SliderFloat("mu",&modifier.mu,0.0f,1.0f);
    ImGui::SliderFloat("alpha",&modifier.alpha,0.0f,1.0f);
    ImGui::SliderFloat("beta",&modifier.beta,0.0f,1.0f);
    ImGui::End();

    float f(0);
    if(modifier.existing_vertex.size() > 0)
        calculate_weights(&f,weights,true);
}
