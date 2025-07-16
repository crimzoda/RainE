#include "Node.h"
#include "imgui.h"
#include "imnodes.h"
#include "GraphWindow.h"

Node::Node(NodeType _nodeType, GraphWindow* _graphWindow, const ImVec2 startPos) : nodeType(_nodeType), graphWindow(_graphWindow) {
    graphWindow->highestNodeId++;
    id = graphWindow->highestNodeId;
    printf("[INFO] Created Node with ID %s\n", std::to_string(id).c_str());
    graphWindow->pinCount++;
    inputPinID = graphWindow->pinCount;
    graphWindow->pinCount++;
    outputPinID = graphWindow->pinCount;
    ImNodes::SetNodeScreenSpacePos(id, startPos);
    //printf("input id: %s, output id: %s", std::to_string(inputPinID).c_str(), std::to_string(outputPinID).c_str());
}

void Node::Show() {
    ImNodes::BeginNode(id);
    ImGui::Dummy(ImVec2(80.0f, 45.0f));
    ImNodes::EndNode();
}

void Node::ShowDeleteButton()
{
    if (ImGui::Button("Delete")) {
        bShouldDelete = true;
    }
}
