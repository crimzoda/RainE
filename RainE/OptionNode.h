#pragma once
#include "Node.h"

class OptionNode : public Node {
public:
    Node::Node;
    void Show();

    char inputText[128] = "";
};
