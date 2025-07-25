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
    /*json SerializeAllEvents
    * Returns the json data from ALL EventNodes.
    * This function recursively calls SerializeEvent on each EventNode.
    */
    json SerializeAllEvents(std::vector<std::shared_ptr<EventNode>>& eventNodeList, bool _bIsEditorSave);
    /*json SerializeEvent
    * Returns the json data taken from a specific EventNode.
    * This function recursively calls SerializeNode on each EventNode's children.
    */
    json SerializeEvent(std::shared_ptr<EventNode>& node);
    /*json SerializeNode
    * Seralizes each node by type and then returns it as json.
    */
    json SerializeNode(std::shared_ptr<Node>& node);

    GraphWindow* graphWindow;
    bool bIsEditorSave;

    /*bool LoadEvents
    * Instantiates EventNodes based on the top-level of the json data.
    * Events as keys at the top level of the json, child nodes are inside its value.
    * returns whether load is successful or not.
    */
    bool LoadEvents(std::string filePath);
    /*void LoadNoes
    * Instantiates Nodes based on their 'type' value.
    */
    void LoadNodes(json node_data, std::shared_ptr<Node> parentNode);
};

