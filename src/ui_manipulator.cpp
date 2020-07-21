#include "ui_manipulator.h"
#include "ui/imgui.h"

UIManipulator::UIManipulator()
:weights(0),
is_dirty(false)
{}

UIManipulator::~UIManipulator()
{
    if(weights)
        delete[] weights;
}

static void correct_name(std::string& name)
{
    /// TODO
}

void UIManipulator::setup(std::vector<std::string>& shape_names)
{
    weight_count = shape_names.size();

    for(int i=0;i<weight_count;i++)
    {
        std::string name = shape_names[i];
        correct_name(name);
        names.push_back(name);
    }

    weights = new float[weight_count];
    memset(weights,0,weight_count*sizeof(float));
    is_dirty = false;
}

void UIManipulator::render_ui()
{
    ImGui::Begin("Blendshape sliders");

    for(int i=0;i<weight_count;i++)
    {
        if(ImGui::SliderFloat(names[i].c_str(),&weights[i],0.0f,1.0f))
        {
            is_dirty = true;
        }
    }

    ImGui::End();
}