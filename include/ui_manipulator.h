#ifndef __UI_MANIPULATOR_H__
#define __UI_MANIPULATOR_H__

#include <vector>
#include <string>

class UIManipulator
{
public:
    UIManipulator();
    ~UIManipulator();
    void setup(std::vector<std::string>& shape_names);
    void render_ui();
    
    bool is_dirty;
    std::vector<std::string> names;
    int weight_count;
    float* weights;
};

#endif