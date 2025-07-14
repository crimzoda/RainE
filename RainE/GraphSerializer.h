#pragma once
#include <vector>
#include <memory>
#include "EventNode.h"
#include "DialogNode.h"
#include "ChoiceNode.h"
#include "OptionNode.h"

class GraphSerializer
{
public:
    json SerializeAllEvents(std::vector<std::shared_ptr<EventNode>>& eventNodeList);
    json SerializeEvent(std::shared_ptr<EventNode>& node);
    json SerializeNode(std::shared_ptr<Node>& node);

    GraphWindow* graphWindow;

    void LoadEvents(std::string filePath);
    void LoadNodes(json node_data, std::shared_ptr<Node> parentNode);
};

