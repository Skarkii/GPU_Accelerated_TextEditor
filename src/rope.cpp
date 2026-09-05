#include "rope.hpp"

#include <stdexcept>
#include <cassert>

static size_t utf8Length(const std::string& str) {
    size_t count = 0;
    for (unsigned char c : str) {
        if ((c & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

static size_t utf8ByteOffset(const std::string& str, size_t charPos) {
    size_t bytePos = 0;
    size_t chars = 0;
    while (bytePos < str.size()) {
        if ((static_cast<unsigned char>(str[bytePos]) & 0xC0) != 0x80) {
            if (chars == charPos) {
                return bytePos;
            }
            chars++;
        }
        bytePos++;
    }
    return bytePos;
}

static std::string utf8CharAt(const std::string& str, size_t charPos) {
    size_t byteStart = utf8ByteOffset(str, charPos);
    if (byteStart >= str.size()) return "";

    unsigned char first = str[byteStart];
    size_t len = 1;
    if ((first & 0x80) == 0) len = 1;
    else if ((first & 0xE0) == 0xC0) len = 2;
    else if ((first & 0xF0) == 0xE0) len = 3;
    else if ((first & 0xF8) == 0xF0) len = 4;

    return str.substr(byteStart, len);
}

RopeNode* RopeNode::createLeaf(std::string_view content) {
    RopeNode* node = new RopeNode{};

    node->text = content;
    node->weight = utf8Length(std::string(content));

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


std::string Rope::at(size_t index) const {
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

    return utf8CharAt(node->text, index);
}

static RopeNode* concat(RopeNode* left, RopeNode* right) {
    if (!left) return right;
    if (!right) return left;
    return RopeNode::createInternal(left, right);
}

static std::pair<RopeNode*, RopeNode*> split(RopeNode* node, size_t pos) {
    if (!node) return {nullptr, nullptr};

    if (node->isLeaf()) {
        size_t bytePos = utf8ByteOffset(node->text, pos);
        std::string leftText = node->text.substr(0, bytePos);
        std::string rightText = node->text.substr(bytePos);

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
