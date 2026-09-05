#pragma once

#include <string>
#include <string_view>
#include <cstddef>

enum class Color { Red, Black };

struct RopeNode {
    RopeNode* left = nullptr;
    RopeNode* right = nullptr;
    RopeNode* parent = nullptr;

    Color color = Color::Black;

    size_t weight = 0;

    std::string text;

    [[nodiscard]] bool isLeaf() const noexcept { return !left && !right; }

    static RopeNode* createLeaf(std::string_view content);
    static RopeNode* createInternal(RopeNode* left, RopeNode* right);

};


class Rope {
    RopeNode* root = nullptr;
public:
    Rope() = default;
    ~Rope();

    Rope(const Rope&) = delete;
    Rope& operator=(const Rope&) = delete;
    Rope(Rope&& other) noexcept;
    Rope& operator=(Rope&& other) noexcept;
    explicit Rope(std::string_view text);
    explicit Rope(RopeNode* node) : root(node) {}

    void insert(size_t pos, std::string_view text);
    void remove(size_t start, size_t end);
    [[nodiscard]] char at(size_t index) const;
    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
};
