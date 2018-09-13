#include "Parser.h"

auto utf8Decode(MemoryView data) -> Generator<FileChar> {
    auto hasData = [&](size_t bytes = 1) {
        return data.begin + bytes <= data.end;
    };
    auto take = [&]() -> CodePoint {
        return *data.begin++;
    };

    while (hasData()) {
        auto chr = [ptr = data.begin](CodePoint cp) {
            return FileChar{cp, ptr};
        };
        auto c0 = take();
        if ((c0 & 0x80) != 0x80)
            co_yield chr(c0);
        else if ((c0 & 0xE0) == 0xC0) {
            if (!hasData(1)) co_return;
            auto c1 = take();
            if ((c1 & 0xC0) != 0x80)
                co_yield chr({});
            else
                co_yield chr(((c0 & 0x1Fu) << 6) |
                             ((c1 & 0x3Fu) << 0));
        }
        else if ((c0 & 0xF0) == 0xE0) {
            if (!hasData(2)) co_return;
            auto c1 = take();
            if ((c1 & 0xC0) != 0x80) {
                co_yield chr({});
                continue;
            }
            auto c2 = take();
            if ((c2 & 0xC0) != 0x80)
                co_yield chr({});
            else
                co_yield chr(((c0 & 0x0Fu) << 12) |
                             ((c1 & 0x3Fu) << 6) |
                             ((c2 & 0x3Fu) << 0));
        }
        else if ((c0 & 0xF8) == 0xF0) {
            if (!hasData(3)) co_return;
            auto c1 = take();
            if ((c1 & 0xC0) != 0x80) {
                co_yield chr({});
                continue;
            }
            auto c2 = take();
            if ((c2 & 0xC0) != 0x80) {
                co_yield chr({});
                continue;
            }
            auto c3 = take();
            if ((c3 & 0xC0) != 0x80)
                co_yield chr({});
            else
                co_yield chr(((c0 & 0x07u) << 18) |
                             ((c1 & 0x3Fu) << 12) |
                             ((c2 & 0x3Fu) << 6) |
                             ((c3 & 0x3Fu) << 0));
        }
        else
            co_yield chr({});
    }
}

#define TAB_WIDTH 4u

auto scan(const FileContent &file,
          Generator<FileChar> input) -> Generator<Token> {
    auto cursor = TextPosition{};
    auto next = [&] {
        if (++input) return *input;
        return FileChar{};
    };

    auto fc = next();
    auto &cp = fc.codePoint;
    while (cp) {
        cursor.ptr = fc.ptr;
        auto token = [begin = cursor, &cursor,
                      &file](auto &&variant) {
            return Token{
                TextRange{&file, begin, cursor},
                std::forward<decltype(variant)>(variant)};
        };

        auto isNewLine = [&] {
            return cp == '\n' || cp == '\r';
        };
        if (isNewLine()) {
            cursor.row++;
            cursor.column = {};
            auto tmp = fc.codePoint;
            fc = next();
            if (fc.codePoint != tmp && isNewLine())
                fc = next();
            co_yield token(NewLine{});
            continue;
        }
        auto isWhiteSpace = [&] {
            return cp == ' ' || cp == '\t';
        };
        if (isWhiteSpace()) {
            do {
                if (cp == '\t')
                    cursor.column |= (TAB_WIDTH - 1);
                cursor.column++;
                fc = next();
            } while (isWhiteSpace());
            co_yield token(WhiteSpace{});
            continue;
        }

        auto single = [&](auto v) {
            cursor.column++;
            fc = next();
            return token(std::move(v));
        };
        switch (cp) {
        case ':': co_yield single(Colon{}); continue;
        case ',': co_yield single(Comma{}); continue;
        case '(': co_yield single(ParenOpen{}); continue;
        case ')': co_yield single(ParenClose{}); continue;
        case '=': co_yield single(Equals{}); continue;
        case '+': co_yield single(Plus{}); continue;
        case '-': co_yield single(Minus{}); continue;
        case '*': co_yield single(Multiply{}); continue;
        case '/': co_yield single(Divide{}); continue;
        }

        auto isDigit = [&] {
            return cp >= '0' && cp <= '9';
        };
        if (isDigit()) {
            uint64_t value = 0;
            do {
                // TODO: protect overflow
                value = (value * 10) + (cp - '0');
                cursor.column++;
                fc = next();
            } while (isDigit());
            co_yield token(Number{value});
            continue;
        }

        auto isIdentChar = [&] {
            return (cp >= 'a' && cp <= 'z') ||
                   (cp >= 'A' && cp <= 'Z') || cp == '_';
        };
        if (isIdentChar()) {
            auto b = fc.ptr;
            do {
                cursor.column++;
                fc = next();
            } while (isIdentChar() || isDigit());
            auto intern = b; // TODO
            co_yield token(Ident{intern});
            continue;
        }
    }
}
