#pragma once
#include "Node.h"

class EventNode : public Node {
public:
    Node::Node;
    void Show();

    char inputText[128] = "";
};
