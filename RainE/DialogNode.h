#pragma once
#include "Node.h"

class DialogNode : public Node {
public:
    Node::Node;
    void Show();
    char characterText[128] = "";
    char inputText[128] = "";
    int currentSelected = 0;
};
