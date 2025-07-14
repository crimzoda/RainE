#include "OptionNode.h"
#include "imgui.h"
#include "imnodes.h"

void OptionNode::Show()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(153, 102, 51, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(153, 102, 51, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(153, 102, 51, 255));
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Option");
    ImNodes::EndNodeTitleBar();
    ImGui::PushItemWidth(100);
    ImGui::InputText("##Input", inputText, IM_ARRAYSIZE(inputText));
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(80.0f, 45.0f));

    ImNodes::BeginInputAttribute(inputPinID);
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(outputPinID);
    ImNodes::EndOutputAttribute();

    ShowDeleteButton();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
