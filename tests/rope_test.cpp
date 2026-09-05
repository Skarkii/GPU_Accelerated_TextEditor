#include <catch2/catch_test_macros.hpp>
#include "rope.hpp"

TEST_CASE("Rope::at throws on empty rope", "[rope]") {
    Rope rope;
    REQUIRE_THROWS_AS(rope.at(0), std::out_of_range);
}

TEST_CASE("Rope::at works with internal nodes", "[rope]") {
    auto* left = RopeNode::createLeaf("Hello ");
    auto* right = RopeNode::createLeaf("World");
    auto* root = RopeNode::createInternal(left, right);

    Rope rope(root);

    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(5) == " ");
    REQUIRE(rope.at(6) == "W");
    REQUIRE(rope.at(10) == "d");
}

TEST_CASE("Insert into empty rope", "[rope]") {
    Rope rope;
    rope.insert(0, "Hello");
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(4) == "o");
}

TEST_CASE("Insert at beginning", "[rope]") {
    Rope rope("World");
    rope.insert(0, "Hello ");
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(6) == "W");
}

TEST_CASE("Insert at end", "[rope]") {
    Rope rope("Hello");
    rope.insert(5, " World");
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(6) == "W");
}

TEST_CASE("Insert in middle", "[rope]") {
    Rope rope("HelloWorld");
    rope.insert(5, " ");
    REQUIRE(rope.at(4) == "o");
    REQUIRE(rope.at(5) == " ");
    REQUIRE(rope.at(6) == "W");
}

TEST_CASE("Multiple inserts build deep tree", "[rope]") {
    Rope rope("A");
    rope.insert(1, "B");  // "AB"
    rope.insert(2, "C");  // "ABC"
    rope.insert(3, "D");  // "ABCD"
    rope.insert(4, "E");  // "ABCDE"
    rope.insert(5, "F");  // "ABCDEF"

    REQUIRE(rope.at(0) == "A");
    REQUIRE(rope.at(1) == "B");
    REQUIRE(rope.at(2) == "C");
    REQUIRE(rope.at(3) == "D");
    REQUIRE(rope.at(4) == "E");
    REQUIRE(rope.at(5) == "F");
}

TEST_CASE("Multiple inserts at various positions", "[rope]") {
    Rope rope("Hello World");
    rope.insert(5, ",");           // "Hello, World" (12 chars)
    rope.insert(7, "beautiful ");  // "Hello, beautiful World" (22 chars)
    rope.insert(0, "Oh! ");        // "Oh! Hello, beautiful World" (26 chars)
    rope.insert(26, "!");          // "Oh! Hello, beautiful World!" (27 chars)

    REQUIRE(rope.at(0) == "O");
    REQUIRE(rope.at(4) == "H");
    REQUIRE(rope.at(9) == ",");
    REQUIRE(rope.at(11) == "b");
    REQUIRE(rope.at(26) == "!");
}

TEST_CASE("Build sentence word by word", "[rope]") {
    Rope rope;
    rope.insert(0, "The");
    rope.insert(3, " quick");
    rope.insert(9, " brown");
    rope.insert(15, " fox");
    rope.insert(19, " jumps");

    // "The quick brown fox jumps"
    REQUIRE(rope.at(0) == "T");
    REQUIRE(rope.at(4) == "q");
    REQUIRE(rope.at(10) == "b");
    REQUIRE(rope.at(16) == "f");
    REQUIRE(rope.at(20) == "j");
    REQUIRE(rope.at(24) == "s");
}

TEST_CASE("Insert single characters repeatedly", "[rope]") {
    Rope rope;
    rope.insert(0, "a");
    rope.insert(0, "b");
    rope.insert(0, "c");
    rope.insert(0, "d");
    rope.insert(0, "e");

    // "edcba"
    REQUIRE(rope.at(0) == "e");
    REQUIRE(rope.at(1) == "d");
    REQUIRE(rope.at(2) == "c");
    REQUIRE(rope.at(3) == "b");
    REQUIRE(rope.at(4) == "a");
}

// === Remove tests ===

TEST_CASE("Remove from beginning", "[rope]") {
    Rope rope("Hello World");
    rope.remove(0, 6);  // Remove "Hello "

    REQUIRE(rope.at(0) == "W");
    REQUIRE(rope.at(4) == "d");
}

TEST_CASE("Remove from end", "[rope]") {
    Rope rope("Hello World");
    rope.remove(5, 11);  // Remove " World"

    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(4) == "o");
    REQUIRE_THROWS_AS(rope.at(5), std::out_of_range);
}

TEST_CASE("Remove from middle", "[rope]") {
    Rope rope("Hello World");
    rope.remove(5, 6);  // Remove " "

    // "HelloWorld"
    REQUIRE(rope.at(4) == "o");
    REQUIRE(rope.at(5) == "W");
}

TEST_CASE("Remove single character", "[rope]") {
    Rope rope("Hello");
    rope.remove(2, 3);  // Remove "l"

    // "Helo"
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(1) == "e");
    REQUIRE(rope.at(2) == "l");
    REQUIRE(rope.at(3) == "o");
}

TEST_CASE("Remove everything", "[rope]") {
    Rope rope("Hello");
    rope.remove(0, 5);

    REQUIRE_THROWS_AS(rope.at(0), std::out_of_range);
}

TEST_CASE("Remove after multiple inserts", "[rope]") {
    Rope rope("AC");
    rope.insert(1, "B");  // "ABC"
    rope.remove(1, 2);     // Remove "B"

    // "AC"
    REQUIRE(rope.at(0) == "A");
    REQUIRE(rope.at(1) == "C");
}

// === Size and empty tests ===

TEST_CASE("Empty rope has size 0", "[rope]") {
    Rope rope;
    REQUIRE(rope.size() == 0);
    REQUIRE(rope.empty());
}

TEST_CASE("Size of single leaf", "[rope]") {
    Rope rope("Hello");
    REQUIRE(rope.size() == 5);
    REQUIRE_FALSE(rope.empty());
}

TEST_CASE("Size after inserts", "[rope]") {
    Rope rope("Hello");
    REQUIRE(rope.size() == 5);

    rope.insert(5, " World");
    REQUIRE(rope.size() == 11);

    rope.insert(0, "Oh! ");
    REQUIRE(rope.size() == 15);
}

TEST_CASE("Size after removes", "[rope]") {
    Rope rope("Hello World");
    REQUIRE(rope.size() == 11);

    rope.remove(5, 11);  // Remove " World"
    REQUIRE(rope.size() == 5);

    rope.remove(0, 5);  // Remove everything
    REQUIRE(rope.size() == 0);
    REQUIRE(rope.empty());
}

// === Unicode tests ===

TEST_CASE("Unicode size counts characters not bytes", "[rope][unicode]") {
    Rope rope("åäö");  // 3 characters, but 6 bytes in UTF-8
    REQUIRE(rope.size() == 3);
}

TEST_CASE("Unicode at returns correct character", "[rope][unicode]") {
    Rope rope("Hållö");  // H å l l ö
    REQUIRE(rope.size() == 5);
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(1) == "å");
    REQUIRE(rope.at(2) == "l");
    REQUIRE(rope.at(3) == "l");
    REQUIRE(rope.at(4) == "ö");
}

TEST_CASE("Unicode insert", "[rope][unicode]") {
    Rope rope("Hllo");
    rope.insert(1, "ä");  // Insert ä at position 1
    // "Hällo"
    REQUIRE(rope.size() == 5);
    REQUIRE(rope.at(1) == "ä");
}

TEST_CASE("Unicode remove", "[rope][unicode]") {
    Rope rope("Hållö");
    rope.remove(1, 2);  // Remove å
    // "Hllö"
    REQUIRE(rope.size() == 4);
    REQUIRE(rope.at(0) == "H");
    REQUIRE(rope.at(1) == "l");
}

TEST_CASE("Mixed ASCII and Unicode", "[rope][unicode]") {
    Rope rope("Hello Världen");  // "Hello World" in Swedish
    REQUIRE(rope.size() == 13);  // 13 characters
    REQUIRE(rope.at(6) == "V");
    REQUIRE(rope.at(7) == "ä");
}
