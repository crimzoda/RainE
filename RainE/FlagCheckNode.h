#pragma once
#include "Node.h"

class FlagCheckNode : public Node {
public:
    Node::Node;
    void Show();
    char flagText[128] = "";
    bool bCondition = true;
};
