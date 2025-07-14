#pragma once
#include "Node.h"

class ChoiceNode : public Node {
public:
    Node::Node;

    bool bRepeat = false;

    void Show();
};
