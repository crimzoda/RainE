#include "ChoiceNode.h"
#include "imgui.h"
#include "imnodes.h"

void ChoiceNode::Show()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 204, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(255, 204, 0, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(255, 204, 0, 255));
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    ImGui::TextUnformatted("Choice");
    ImGui::PopStyleColor();
    ImNodes::EndNodeTitleBar();
    ImGui::Checkbox("Repeat", &bRepeat);
    ImGui::Dummy(ImVec2(80.0f, 20.0f));

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
