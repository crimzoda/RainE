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
    Node(NodeType _nodeType, int _id, GraphWindow* _graphWindow, ImVec2 startPos);
    virtual void Show();
    void ShowDeleteButton();

    NodeType nodeType;
    GraphWindow* graphWindow;

    std::vector<std::shared_ptr<Node>> children;

    int linkCount = 0;
    bool bShouldDelete = false;

    int id;
    int inputPinID;
    int outputPinID;
};

