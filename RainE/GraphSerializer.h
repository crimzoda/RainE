#pragma once
#include "json_fwd.hpp"

#include <vector>
#include <memory>

class Node;
class EventNode;
class GraphWindow;

using json = nlohmann::json;

class GraphSerializer
{
public:
    json SerializeAllEvents(std::vector<std::shared_ptr<EventNode>>& eventNodeList, bool _bInclPositions);
    json SerializeEvent(std::shared_ptr<EventNode>& node);
    json SerializeNode(std::shared_ptr<Node>& node);

    GraphWindow* graphWindow;
    bool bInclPositions;

    bool LoadEvents(std::string filePath);
    void LoadNodes(json node_data, std::shared_ptr<Node> parentNode);
};

