#pragma once
#include <cinttypes>
#include <string_view>
#include <variant>

#include "Generator.h"

// input
using Byte = uint8_t;
using StringView = std::string_view;

struct MemoryView {
    const Byte *begin;
    const Byte *end;
};

struct FileContent {
    StringView name;
    MemoryView content;
};

using CodePoint = uint32_t;

struct FileChar {
    CodePoint codePoint;
    const Byte *ptr;
};

auto utf8Decode(MemoryView data) -> Generator<FileChar>;

// output

struct TextPosition {
    size_t row{};    // 0 based
    size_t column{}; // 0 based
    const Byte *ptr{};
};
struct TextRange {
    const FileContent *file{};
    TextPosition begin{};
    TextPosition end{};
};
template<class Tag>
struct Tagged {
    auto operator==(const Tagged &) const { return true; }
    auto operator!=(const Tagged &b) const {
        return !(*this == b);
    }
};

// NOLINTNEXTLINE(misc-macro-parentheses)
#define TAGGED(name) using name = Tagged<struct name##Tag>

TAGGED(NewLine);
TAGGED(WhiteSpace);
TAGGED(Comment);
TAGGED(Colon);
TAGGED(Comma);
TAGGED(ParenOpen);
TAGGED(ParenClose);
TAGGED(Equals);
TAGGED(Plus);
TAGGED(Minus);
TAGGED(Multiply);
TAGGED(Divide);
// TAGGED(String);

#undef TAGGED

struct Number {
    uint64_t value;

    bool operator==(const Number &b) const {
        return value == b.value;
    }
    bool operator!=(const Number &b) const {
        return !(*this == b);
    }
};

struct Ident {
    const Byte *reference;

    bool operator==(const Ident &b) const {
        return reference == b.reference;
    }
    bool operator!=(const Ident &b) const {
        return !(*this == b);
    }
};

using TokenVariant =
    std::variant<NewLine, WhiteSpace, Colon, Comma,
                 ParenOpen, ParenClose, Equals, Plus, Minus,
                 Multiply, Divide, Number, Ident>;

struct Token {
    TextRange range;
    TokenVariant kind;
};

auto scan(const FileContent &, Generator<FileChar>)
    -> Generator<Token>;
