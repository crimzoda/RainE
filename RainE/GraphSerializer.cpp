#include "GraphSerializer.h"
#include "json.hpp"
#include "json_fwd.hpp"
#include <iostream>
#include <fstream>


using json = nlohmann::json;

json GraphSerializer::SerializeAllEvents(std::vector<std::shared_ptr<EventNode>>& eventNodeList)
{
    json j = {};

    for (auto& eventNode : eventNodeList) {
        ImVec2 eventNodePos = ImNodes::GetNodeEditorSpacePos(eventNode->id);
        std::string eventName = eventNode->inputText;
        j[eventName.append('[' + std::to_string(eventNodePos.x) + "," + std::to_string(eventNodePos.y) + "]")] = SerializeEvent(eventNode);
        std::cout << eventName << std::endl;
    }

    return j;
}

json GraphSerializer::SerializeEvent(std::shared_ptr<EventNode>& node) {
    json jEventData = {};
    for (int i = 0; i < node->children.size(); i++) {
        jEventData.push_back(SerializeNode(node->children[i]));
    }
    return jEventData;
}

json GraphSerializer::SerializeNode(std::shared_ptr<Node>& node)
{
    json jNodeData = {};
    std::cout << std::to_string(node->id) << std::endl;
    if (node->nodeType == NodeType::Dialog) {
        std::shared_ptr<DialogNode> dialogNode = std::dynamic_pointer_cast<DialogNode>(node);
        jNodeData["type"] = "dialogue";
        jNodeData["character"] = dialogNode->characterText;
        jNodeData["dialogue"] = dialogNode->inputText;
    }
    if (node->nodeType == NodeType::Choice) {
        std::shared_ptr<ChoiceNode> choiceNode = std::dynamic_pointer_cast<ChoiceNode>(node);
        jNodeData["type"] = "choice";
        jNodeData["repeat"] = choiceNode->bRepeat;
        jNodeData["options"] = {};

        for (int i = 0; i < choiceNode->children.size(); i++) {
            jNodeData["options"].push_back(SerializeNode(choiceNode->children[i]));
        }
    }
    if (node->nodeType == NodeType::Option) {
        std::shared_ptr<OptionNode> optionNode = std::dynamic_pointer_cast<OptionNode>(node);
        jNodeData["type"] = "option";
        jNodeData["dialogue"] = optionNode->inputText;
        jNodeData["branch"] = {};

        for (int i = 0; i < optionNode->children.size(); i++) {
            jNodeData["branch"].push_back(SerializeNode(optionNode->children[i]));
        }
    }

    jNodeData["pos"] = { ImNodes::GetNodeEditorSpacePos(node->id).x, ImNodes::GetNodeEditorSpacePos(node->id).y };

    return jNodeData;
}

void GraphSerializer::LoadEvents(std::string filePath)
{
    std::ifstream f(filePath);
    json data = json::parse(f);

    std::cout << data.dump(4);

    for (auto& el : data.items()) {
        int id = (graphWindow->nodeList.size() > 0) ? graphWindow->nodeList.back()->id + 1 : 1;

        //Hacky as hell - TODO: refactor to giving event nodes json properties like all other nodes
        std::string position = el.key().substr(el.key().find("["), el.key().find("]"));
        float pos_x = std::stof(position.substr(1, position.find(",")));
        float pos_y = std::stof(position.substr(position.find(",") + 1, position.length()));

        std::shared_ptr<EventNode> eventNode = std::make_shared<EventNode>(NodeType::Event, id, graphWindow, ImVec2(pos_x, pos_y));
        strcpy_s(eventNode->inputText, el.key().substr(0, el.key().find_first_of('[')).c_str());
        graphWindow->nodeList.push_back(eventNode);
        graphWindow->eventNodeList.push_back(eventNode);

        for (auto& node : el.value()) {
            LoadNodes(node, eventNode);
        }
    }
}

void GraphSerializer::LoadNodes(json node_data, std::shared_ptr<Node> parentNode) {
    std::shared_ptr<Node> finalNode;
    int id = (graphWindow->nodeList.size() > 0) ? graphWindow->nodeList.back()->id + 1 : 1;
    ImVec2 position = ImVec2(node_data["pos"][0], node_data["pos"][1]);

    //TODO: Could probably optimize this, lots of repetition with the id, node creation and nodeList pushes.
    if (node_data["type"] == "dialogue") {
        std::shared_ptr<DialogNode> dialogNode = std::make_shared<DialogNode>(NodeType::Dialog, id, graphWindow, position);
        strcpy_s(dialogNode->characterText, node_data["character"].get<std::string>().c_str());
        strcpy_s(dialogNode->inputText, node_data["dialogue"].get<std::string>().c_str());

        graphWindow->nodeList.push_back(dialogNode);
        finalNode = dialogNode;
    }
    if (node_data["type"] == "choice") {
        std::shared_ptr<ChoiceNode> choiceNode = std::make_shared<ChoiceNode>(NodeType::Choice, id, graphWindow, position);
        choiceNode->bRepeat = node_data["repeat"];
        graphWindow->nodeList.push_back(choiceNode);

        for (int i = 0; i < node_data["options"].size(); i++) {
            LoadNodes(node_data["options"][i], choiceNode);
        }

        finalNode = choiceNode;
    }
    if (node_data["type"] == "option") {
        std::shared_ptr<OptionNode> optionNode = std::make_shared<OptionNode>(NodeType::Option, id, graphWindow, position);
        strcpy_s(optionNode->inputText, node_data["dialogue"].get<std::string>().c_str());
        graphWindow->nodeList.push_back(optionNode);

        for (int i = 0; i < node_data["branch"].size(); i++) {
            LoadNodes(node_data["branch"][i], optionNode);
        }

        finalNode = optionNode;
    }

    Link link;
    link.id = graphWindow->links.size() + 1;
    link.start_attr = parentNode->outputPinID;
    link.end_attr = finalNode->inputPinID;
    graphWindow->links.push_back(link);
}
