#include "FlagCheckNode.h"
#include "imgui.h"
#include "imnodes.h"

void FlagCheckNode::Show()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(161, 0, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(161, 0, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(161, 0, 0, 255));
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Flag Check");
    ImNodes::EndNodeTitleBar();
    ImGui::PushItemWidth(150);
    ImGui::Text("Flag:");
    ImGui::InputText("##Input", flagText, IM_ARRAYSIZE(flagText));
    ImGui::Text("Dialog:");
    ImGui::Checkbox("Condition", &bCondition);
    ImGui::PopItemWidth();

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
