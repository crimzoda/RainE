#pragma once

#include <string>
#include <vector>
#include <memory>
#include "GraphWindow.h"

enum class NodeType {
    Event,
    Dialog,
    Choice,
    Option,
    FlagCheck
};

class Node {
public:
    Node(NodeType _nodeType, GraphWindow* _graphWindow, const ImVec2 startPos);
    virtual void Show();
    void ShowDeleteButton();

    NodeType nodeType;
    //Used for up to date referencing of pinCount member variable of graphWindow
    GraphWindow* graphWindow;

    //Stores child nodes, useful for looping through when saving and loading
    std::vector<std::shared_ptr<Node>> children;

    int linkCount = 0;
    bool bShouldDelete = false;

    int id;
    int inputPinID;
    int outputPinID;
};

