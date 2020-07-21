#ifndef __BLENDSHAPE_ANIMATOR_H__
#define __BLENDSHAPE_ANIMATOR_H__

#define SHAPE_COUNT 24

struct Keyframe
{
    float weights[SHAPE_COUNT];
};

class BlendshapeAnimator
{
    Keyframe* frames;
    int keyframe_count;
    
    float delta;
    double start_time;
    double stop_time;
public:
    BlendshapeAnimator();
    ~BlendshapeAnimator();
    void read_animation(const char* filepath);
    
    void start_animation(double time,float in_delta);
    void update(double time);
    void stop_animation();
    
    bool is_playing;
    float weights[SHAPE_COUNT];
};

#endif
