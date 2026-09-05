#include "rope.hpp"

#include <stdexcept>
#include <cassert>

RopeNode* RopeNode::createLeaf(std::string_view content) {
    RopeNode* node = new RopeNode{};

    node->text = content;
    node->weight = content.size();

    return node;
}

static size_t subtreeLength(const RopeNode* node) {
    if (!node) return 0;
    if (node->isLeaf()) return node->weight;
    return subtreeLength(node->left) + subtreeLength(node->right);
}

RopeNode* RopeNode::createInternal(RopeNode* left, RopeNode* right) {
    RopeNode* node = new RopeNode{};

    node->left = left;
    if (left) left->parent = node;
    node->right = right;
    if (right) right->parent = node;

    node->weight = subtreeLength(left);

    return node;
}

static void deleteSubTree(RopeNode* node) {
    if (!node) return;

    deleteSubTree(node->left);
    deleteSubTree(node->right);
    delete node;
}

Rope::~Rope() {
    deleteSubTree(root);
}

Rope::Rope(Rope&& other) noexcept 
    : root(other.root) {
    other.root = nullptr;
}

Rope& Rope::operator=(Rope&& other) noexcept {
    if (this != &other) {
        deleteSubTree(root);
        root = other.root;
        other.root = nullptr;
    }
    return *this;
}

Rope::Rope(std::string_view text) {
    if (!text.empty()) {
        root = RopeNode::createLeaf(text);
    }
}

char Rope::at(size_t index) const {
    if (!root) {
        throw std::out_of_range("empty rope");
    }

    if (index >= size()) {
        throw std::out_of_range("index out of bounds");
    }

    RopeNode* node = root;

    while (!node->isLeaf()){
        if (node->weight > index) {
            node = node->left;
        } else {
            index -= node->weight;
            node = node->right;
        }
        assert(node != nullptr);
    }

    return node->text.at(index);
}

static RopeNode* concat(RopeNode* left, RopeNode* right) {
    if (!left) return right;
    if (!right) return left;
    return RopeNode::createInternal(left, right);
}

static std::pair<RopeNode*, RopeNode*> split(RopeNode* node, size_t pos) {
    if (!node) return {nullptr, nullptr};

    if (node->isLeaf()) {
        std::string leftText = node->text.substr(0, pos);
        std::string rightText = node->text.substr(pos);

        RopeNode* leftLeaf = leftText.empty() ? nullptr : RopeNode::createLeaf(leftText);
        RopeNode* rightLeaf = rightText.empty() ? nullptr : RopeNode::createLeaf(rightText);

        delete node;
        return {leftLeaf, rightLeaf};
    }

    if (pos < node->weight) {
        RopeNode* rightChild = node->right;
        auto [leftLeft, leftRight] = split(node->left, pos);
        delete node;
        return {leftLeft, concat(leftRight, rightChild)};
    }
    else {
        RopeNode* leftChild = node->left;
        auto [rightLeft, rightRight] = split(node->right, pos - node->weight);
        delete node;
        return {concat(leftChild, rightLeft), rightRight};
    }
}

void Rope::insert(size_t pos, std::string_view text) {
    if (text.empty()) return;

    if (!root) {
        root = RopeNode::createLeaf(text);
        return;
    }

    auto [left, right] = split(root, pos);
    RopeNode* middle = RopeNode::createLeaf(text);
    root = concat(concat(left, middle), right);
}


void Rope::remove(size_t start, size_t end) {
    if (start > end || !root) return;

    auto [left, rest] = split(root, start);
    auto [middle, right] = split(rest, end - start);

    deleteSubTree(middle);

    root = concat(left, right);
}

size_t Rope::size() const noexcept {
    return subtreeLength(root);
}

bool Rope::empty() const noexcept {
    return root == nullptr;
}
