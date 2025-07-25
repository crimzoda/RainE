#include "CharacterListNode.h"
#include "imgui.h"
#include "imnodes.h"

void CharacterListNode::Show()
{
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Character List");
    ImNodes::EndNodeTitleBar();
    ImGui::PushItemWidth(100);
    ImGui::InputText("##Input", nameInput, IM_ARRAYSIZE(nameInput));
    ImGui::SameLine();
    if (ImGui::Button("Add Character")) {
        auto it = std::find(graphWindow->characterList.begin(), graphWindow->characterList.end(), nameInput);
        if (it == graphWindow->characterList.end()) {
            graphWindow->characterList.push_back(nameInput);
        }
        
    }
    ImGui::BeginChild(1, ImVec2(200, 400));
    for (auto& character : graphWindow->characterList) {
        ShowCharacterItem(character);
    }
    ImGui::EndChild();
    
    ImNodes::EndNode();
}

void CharacterListNode::ShowCharacterItem(std::string name) {
    ImGui::Text(name.c_str());
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        DeleteCharacter(name);
    }
}

void CharacterListNode::DeleteCharacter(std::string character)
{
    for (int i = 0; i < graphWindow->characterList.size(); i++) {
        if (graphWindow->characterList[i] == character) {
            graphWindow->characterList.erase(graphWindow->characterList.begin() + i);
        }
    }
}
