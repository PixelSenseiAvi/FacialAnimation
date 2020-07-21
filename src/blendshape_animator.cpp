#include "blendshape_animator.h"

#include <glm/glm.hpp>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

BlendshapeAnimator::BlendshapeAnimator()
:frames(0),
is_playing(false),
start_time(0),
stop_time(0)
{}

BlendshapeAnimator::~BlendshapeAnimator()
{
    if(frames)
        delete[] frames;
}

void BlendshapeAnimator::read_animation(const char* filepath)
{
    std::ifstream f(filepath);
    
    if(!f.good())
    {
        printf("Error while opening the file");
        f.close();
        return;
    }

    std::vector<float> values;
    while(!f.eof())
    {
        float val;
        f >> val;
        values.push_back(val);
    }
    values.pop_back();
    f.close();
    
    keyframe_count = values.size()/SHAPE_COUNT;
    frames = new Keyframe[keyframe_count];
    memcpy(reinterpret_cast<float*>(&frames[0]),values.data(),values.size() * sizeof(float));
}

void BlendshapeAnimator::start_animation(double time,float in_delta)
{
    delta = in_delta;
    double duration = delta * keyframe_count;
    start_time = time;
    stop_time = time + duration;
    is_playing = true;
    
}
void BlendshapeAnimator::update(double time)
{
    if(time >= stop_time)
    {
        stop_animation();
        return;
    }
    
    double temp = (time - start_time)/delta;
    int t2 = glm::floor(temp) - 1;
    int t1 = glm::ceil(temp) - 1;

    for(int i=0;i<SHAPE_COUNT;i++)
    {
        /// Linear interpolation
       weights[i] = (frames[t1].weights[i] + frames[t2].weights[i]) * 0.5f;
    }
}

void BlendshapeAnimator::stop_animation()
{
    memcpy(weights,&frames[0],SHAPE_COUNT * sizeof(float));
    is_playing = false;
}
