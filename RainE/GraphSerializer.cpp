#include "GraphSerializer.h"
#include "json.hpp"
#include "json_fwd.hpp"
#include <iostream>
#include <fstream>


using json = nlohmann::json;

json GraphSerializer::SerializeAllEvents(std::vector<std::shared_ptr<EventNode>>& eventNodeList, bool _bInclPositions)
{
    bInclPositions = _bInclPositions;

    json j = {};

    for (auto& eventNode : eventNodeList) {
        ImVec2 eventNodePos = ImNodes::GetNodeEditorSpacePos(eventNode->id);
        std::string eventName = eventNode->inputText;
        std::string metadata = bInclPositions ? '[' + std::to_string(eventNodePos.x) + "," + std::to_string(eventNodePos.y) + "]" : "";
        j[eventName.append(metadata)] = SerializeEvent(eventNode);
        std::cout << eventName << std::endl;
    }

    return j;
}

json GraphSerializer::SerializeEvent(std::shared_ptr<EventNode>& node) {
    json jEventData = {};
    for (auto& subNode : node->children) {
        jEventData.push_back(SerializeNode(subNode));
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

        for (auto& option : choiceNode->children) {
            jNodeData["options"].push_back(SerializeNode(option));
        }
    }
    if (node->nodeType == NodeType::Option) {
        std::shared_ptr<OptionNode> optionNode = std::dynamic_pointer_cast<OptionNode>(node);
        jNodeData["type"] = "option";
        jNodeData["dialogue"] = optionNode->inputText;
        jNodeData["branch"] = {};

        for (auto& node : optionNode->children) {
            jNodeData["branch"].push_back(SerializeNode(node));
        }
    }
    if (node->nodeType == NodeType::FlagCheck) {
        std::shared_ptr<FlagCheckNode> flagCheckNode = std::dynamic_pointer_cast<FlagCheckNode>(node);
        jNodeData["type"] = "flag_check";
        jNodeData["flag"] = flagCheckNode->flagText;
        jNodeData["condition"] = flagCheckNode->bCondition;
        jNodeData["branch"] = {};

        for (auto& node : flagCheckNode->children) {
            jNodeData["branch"].push_back(SerializeNode(node));
        }
    }

    if (bInclPositions)
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

        for (auto& option : node_data["options"]) {
            LoadNodes(option, choiceNode);
        }

        finalNode = choiceNode;
    }
    if (node_data["type"] == "option") {
        std::shared_ptr<OptionNode> optionNode = std::make_shared<OptionNode>(NodeType::Option, id, graphWindow, position);
        strcpy_s(optionNode->inputText, node_data["dialogue"].get<std::string>().c_str());
        graphWindow->nodeList.push_back(optionNode);

        for (auto& node : node_data["branch"]) {
            LoadNodes(node, optionNode);
        }

        finalNode = optionNode;
    }
    if (node_data["type"] == "flag_check") {
        std::shared_ptr<FlagCheckNode> flagCheckNode = std::make_shared<FlagCheckNode>(NodeType::Option, id, graphWindow, position);
        strcpy_s(flagCheckNode->flagText, node_data["flag"].get<std::string>().c_str());
        flagCheckNode->bCondition = node_data["condition"];
        graphWindow->nodeList.push_back(flagCheckNode);

        for (auto& node : node_data["branch"]) {
            LoadNodes(node, flagCheckNode);
        }

        finalNode = flagCheckNode;
    }

    Link link;
    link.id = graphWindow->links.size() + 1;
    link.start_attr = parentNode->outputPinID;
    link.end_attr = finalNode->inputPinID;
    graphWindow->links.push_back(link);
}
