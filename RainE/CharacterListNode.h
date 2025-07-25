#pragma once
#include "Node.h"

class CharacterListNode : public Node {
public:
    Node::Node;

    char nameInput[128] = "";

    void Show();
    void ShowCharacterItem(std::string name);
    void DeleteCharacter(std::string character);
};
