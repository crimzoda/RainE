#pragma once
#include "imnodes.h"
#include "json_fwd.hpp"
#include "GraphSerializer.h"

#include <memory>
#include <vector>
#include <string>

class Node;
class EventNode;

using json = nlohmann::json;

struct Link {
    int id;
    int start_attr;
    int end_attr;
};

class GraphWindow {
public:
    GraphWindow(bool _bIsLoad, std::string name);
    ~GraphWindow();
    void Update();
    void Show();
    void Save(bool bInclPositions = true);

    GraphSerializer serializer;

    /*std::shared_ptr<Node> GetNodeFromAttribute
    * Returns the Node linked to the attribute.
    * Attribute is the connector pins seen on nodes.
    */
    std::shared_ptr<Node> GetNodeFromAttribute(const int& attr);

    
    ImNodesEditorContext* context = nullptr;
    std::vector<std::shared_ptr<Node>> nodeList;
    std::vector<std::shared_ptr<EventNode>> eventNodeList;
    
    //Ensures unique IDs
    //Previous method of incrementing id of the node at the back did not guarantee this.
    int highestNodeId = 0;
    int highestLinkId = 0;

    std::vector<Link> links;
    int pinCount = 0;
    std::string name;

    bool bIsLoad;
    bool bShouldDelete = false;
};
