#include "DialogNode.h"
#include "imgui.h"
#include "imnodes.h"

void DialogNode::Show()
{
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Dialog");
    ImNodes::EndNodeTitleBar();
    ImGui::PushItemWidth(150);
    ImGui::Text("Character:");
    ImGui::InputText("##Input", characterText, IM_ARRAYSIZE(characterText));
    ImGui::Text("Dialog:");
    ImGui::InputTextMultiline("##Input1", inputText, IM_ARRAYSIZE(inputText));
    ImGui::PopItemWidth();

    ImNodes::BeginInputAttribute(inputPinID);
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(outputPinID);
    ImNodes::EndOutputAttribute();

    ShowDeleteButton();

    ImNodes::EndNode();
}
