#include "GraphWindow.h"
#include "imgui.h"
#include "imnodes.h"
#include "Node.h"
#include "EventNode.h"
#include "DialogNode.h"
#include "ChoiceNode.h"
#include "OptionNode.h"
#include "json.hpp"
#include "json_fwd.hpp"
#include "GraphSerializer.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

GraphSerializer serializer;

GraphWindow::GraphWindow(bool _bIsLoad, std::string _name) : bIsLoad(_bIsLoad), name(_name) {
    serializer.graphWindow = this;

    context = ImNodes::EditorContextCreate();
    ImGui::CreateContext();
}

GraphWindow::~GraphWindow()
{
    ImNodes::EditorContextFree(context);
    ImGui::DestroyContext();
}

void GraphWindow::Show()
{
    ImNodes::EditorContextSet(context);
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin(this->name.c_str());
    
    
    ImNodes::BeginNodeEditor();

    /*Loading is done between ImGui::Begin() and ImNodes::BeginNodeEditor
    otherwise each nodes starting position will not show*/
    if (bIsLoad) {
        serializer.LoadEvents(name);
        size_t dirLastPos = name.find_last_of("/\\") + 1;
        name = name.substr(dirLastPos);
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
            int id = (nodeList.size() > 0) ? nodeList.back()->id + 1 : 1;
            std::shared_ptr<EventNode> eventNode = std::make_shared<EventNode>(NodeType::Event, id, this, ImGui::GetMousePos());
            nodeList.push_back(eventNode);
            eventNodeList.push_back(eventNode);
        }
        if (ImGui::MenuItem("Add Dialog")) {
            int id = (nodeList.size() > 0) ? nodeList.back()->id + 1 : 1;
            std::shared_ptr<DialogNode> dialogNode = std::make_shared<DialogNode>(NodeType::Dialog, id, this, ImGui::GetMousePos());
            nodeList.push_back(dialogNode);
            ImNodes::SetNodeScreenSpacePos(id, ImGui::GetMousePos());
        }
        if (ImGui::MenuItem("Add Choice")) {
            int id = (nodeList.size() > 0) ? nodeList.back()->id + 1 : 1;
            std::shared_ptr<ChoiceNode> choiceNode = std::make_shared<ChoiceNode>(NodeType::Choice, id, this, ImGui::GetMousePos());
            nodeList.push_back(choiceNode);
            ImNodes::SetNodeScreenSpacePos(id, ImGui::GetMousePos());
        }
        if (ImGui::MenuItem("Add Option")) {
            int id = (nodeList.size() > 0) ? nodeList.back()->id + 1 : 1;
            std::shared_ptr<OptionNode> optionNode = std::make_shared<OptionNode>(NodeType::Option, id, this, ImGui::GetMousePos());
            nodeList.push_back(optionNode);
            ImNodes::SetNodeScreenSpacePos(id, ImGui::GetMousePos());
        }
        if (ImGui::MenuItem("Save [TEST]")) {
            this->Save();
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
        link.id = links.size() + 1;
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

void GraphWindow::Save()
{
    json dataToSave = serializer.SerializeAllEvents(eventNodeList);

    std::ofstream file(name);
    file << dataToSave.dump(4);
    file.close();
}

std::shared_ptr<Node> GraphWindow::GetNodeFromAttribute(const int& attr)
{
    for (int i = 0; i < nodeList.size(); i++) {
        if (nodeList[i]->inputPinID == attr || nodeList[i]->outputPinID == attr) {
            return nodeList[i];
        }
    }

    return nullptr;
}

void GraphWindow::Update() {
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
