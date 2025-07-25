#include "GraphWindow.h"
#include "imgui.h"
#include "imnodes.h"
#include "Node.h"
#include "EventNode.h"
#include "DialogNode.h"
#include "ChoiceNode.h"
#include "OptionNode.h"
#include "CharacterListNode.h"
#include "FlagCheckNode.h"
#include "json.hpp"
#include "GraphSerializer.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

GraphWindow::GraphWindow(bool _bIsLoad, const std::string& _name) : bIsLoad(_bIsLoad), name(_name) {
    serializer.graphWindow = this;
    context = ImNodes::EditorContextCreate();
    //Node constructor startPos argument does nothing here because SetNodeScreenSpacePos works only after NodeEditor begins.
    std::shared_ptr<CharacterListNode> characterListNode = std::make_shared<CharacterListNode>(NodeType::CharacterList, this, ImVec2(0, 0));
    nodeList.push_back(characterListNode);
}

GraphWindow::~GraphWindow()
{
    ImNodes::EditorContextFree(context);
}

void GraphWindow::Show()
{
    ImNodes::EditorContextSet(context);
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin(this->name.c_str());
    
    if (ImGui::Button("Close")) {
        bShouldDelete = true;
    }

    ImNodes::BeginNodeEditor();

    /*Loading is done between ImGui::Begin() and ImNodes::BeginNodeEditor
    otherwise each nodes starting position will not show*/
    if (bIsLoad) {
        if (!serializer.LoadEvents(name)) {
            bShouldDelete = true;
        }
            
        //Retrieve file name from directory (if directory)
        size_t dirLastPos = name.find_last_of("/\\") + 1;
        name = name.substr(dirLastPos);
        //Remove file extension (file extension is decided on save not on open)
        name = name.substr(0, name.find_last_of('.'));
        bIsLoad = false;
    }

    const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right);

    const bool delete_links = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Middle);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    if (!ImGui::IsAnyItemHovered() && open_popup) {
        ImGui::OpenPopup("add node");
    }

    //All options
    if (ImGui::BeginPopup("add node")) {
        if (ImGui::MenuItem("Add Event")) {
            std::shared_ptr<EventNode> eventNode = std::make_shared<EventNode>(NodeType::Event, this, ImGui::GetMousePos());
            nodeList.push_back(eventNode);
            eventNodeList.push_back(eventNode);
        }
        if (ImGui::MenuItem("Add Dialog")) {
            std::shared_ptr<DialogNode> dialogNode = std::make_shared<DialogNode>(NodeType::Dialog, this, ImGui::GetMousePos());
            nodeList.push_back(dialogNode);
        }
        if (ImGui::MenuItem("Add Choice")) {
            std::shared_ptr<ChoiceNode> choiceNode = std::make_shared<ChoiceNode>(NodeType::Choice, this, ImGui::GetMousePos());
            nodeList.push_back(choiceNode);
        }
        if (ImGui::MenuItem("Add Option")) {
            std::shared_ptr<OptionNode> optionNode = std::make_shared<OptionNode>(NodeType::Option, this, ImGui::GetMousePos());
            nodeList.push_back(optionNode);
        }
        if (ImGui::MenuItem("Add Flag Check")) {
            std::shared_ptr<FlagCheckNode> flagCheckNode = std::make_shared<FlagCheckNode>(NodeType::FlagCheck, this, ImGui::GetMousePos());
            nodeList.push_back(flagCheckNode);
        }
        if (ImGui::MenuItem("Save")) {
            this->Save();
        }
        if (ImGui::MenuItem("Save as JSON")) {
            /*Sets bInclPositions argument to false
            bInclPositions dictates whether we are saving metadata (only positions of nodes for now)
            This is because we only need node positions in the editor not in-game.*/
            this->Save(false);
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();

    this->Update();

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();

    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
        Link link;
        highestLinkId++;
        link.id = highestLinkId;
        link.start_attr = start_attr;
        link.end_attr = end_attr;
        links.push_back(link);

        //Clear all nodes children before populating
        for (const std::shared_ptr<Node>& node : nodeList) {
            node->children.clear();
            node->linkCount = 0;
        }

        for (int i = 0; i < links.size(); i++) {
            Link _link = links[i];
            std::shared_ptr<Node> parentNode = GetNodeFromAttribute(_link.start_attr);
            std::shared_ptr<Node> childNode = GetNodeFromAttribute(_link.end_attr);

            if (parentNode != nullptr && childNode != nullptr) {
                parentNode->children.push_back(childNode);
                printf("Event ID %s with %s children\n", std::to_string(parentNode->id).c_str(), std::to_string(parentNode->children.size()).c_str());
                parentNode->linkCount++;
                childNode->linkCount++;
                
            }

        }

    }

    //Deleting links...
    if (ImNodes::NumSelectedLinks() > 0) {
        std::vector<int> selectedLinkIds(ImNodes::NumSelectedLinks());
        ImNodes::GetSelectedLinks(selectedLinkIds.data());
        
        if (delete_links) {
            links.erase(std::remove_if(links.begin(), links.end(), [&selectedLinkIds](const Link& link) {
                return std::find(selectedLinkIds.begin(), selectedLinkIds.end(), link.id) != selectedLinkIds.end();
                }), links.end());
        }
        
    }

    ImGui::End();
}

void GraphWindow::Save(bool bIsEditorSave)
{
    json dataToSave = serializer.SerializeAllEvents(eventNodeList, bIsEditorSave);

    //.rg files to be used by the editor, .json to be used by game
    std::string file_name = bIsEditorSave ? name + ".rg" : name + ".json";

    std::ofstream file(file_name);

    if (file) {
        file << dataToSave.dump(4);
    }
    file.close();
}

std::shared_ptr<Node> GraphWindow::GetNodeFromAttribute(const int attr)
{
    for (int i = 0; i < nodeList.size(); i++) {
        if (nodeList[i]->inputPinID == attr || nodeList[i]->outputPinID == attr) {
            return nodeList[i];
        }
    }

    return nullptr;
}

void GraphWindow::Update() {
    //Keeping all node lists up to date as well as handling visibility logic

    for (int i = 0; i < nodeList.size(); i++) {
        if (nodeList[i]->bShouldDelete && nodeList[i]->linkCount == 0) {
            nodeList.erase(nodeList.begin() + i);
        }
        else {
            nodeList[i]->Show();
        }
        
    }

    for (int i = 0; i < eventNodeList.size(); i++) {
        if (eventNodeList[i]->bShouldDelete && eventNodeList[i]->linkCount == 0) {
            eventNodeList.erase(eventNodeList.begin() + i);
        }

    }

    for (int i = 0; i < links.size(); i++) {
        Link _link = links[i];
        ImNodes::Link(_link.id, _link.start_attr, _link.end_attr);

        std::shared_ptr<Node> parentNode = GetNodeFromAttribute(_link.start_attr);
        std::shared_ptr<Node> childNode = GetNodeFromAttribute(_link.end_attr);

        if (parentNode->bShouldDelete || childNode->bShouldDelete) {
            parentNode->linkCount--;
            childNode->linkCount--;
            std::cout << links.size() << ", " << i << std::endl;
            links.erase(links.begin() + i);
       }
    }

}