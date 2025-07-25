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
    std::vector<const char*> _characterList;
    _characterList.reserve(graphWindow->characterList.size());

    for (const auto& _name : graphWindow->characterList) {
        _characterList.push_back(_name.c_str());
    }

    ImGui::Combo("##Combo", &currentSelected, _characterList.data(), _characterList.size());
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
