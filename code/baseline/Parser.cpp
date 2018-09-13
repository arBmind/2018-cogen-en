#include "Parser.h"

auto utf8Decode(MemoryView data) -> Generator<FileChar> {
    struct Utf8Decoder {
        operator bool() const {
            return data.begin < data.end;
        }
        auto next() -> FileChar {

            auto hasData = [&](size_t bytes = 1) {
                return data.begin + bytes <= data.end;
            };
            auto take = [&]() -> CodePoint {
                return *data.begin++;
            };

            auto chr = [ptr = data.begin](CodePoint cp) {
                return FileChar{cp, ptr};
            };
            auto done = [&] {
                data.begin = data.end;
                return FileChar{};
            };
            auto c0 = take();
            if ((c0 & 0x80) != 0x80) {
                return chr(c0);
            }
            if ((c0 & 0xE0) == 0xC0) {
                if (!hasData(1)) return done();
                auto c1 = take();
                if ((c1 & 0xC0) != 0x80) return chr({});
                return chr(((c0 & 0x1Fu) << 6) |
                           ((c1 & 0x3Fu) << 0));
            }
            if ((c0 & 0xF0) == 0xE0) {
                if (!hasData(2)) return done();
                auto c1 = take();
                if ((c1 & 0xC0) != 0x80) return chr({});
                auto c2 = take();
                if ((c2 & 0xC0) != 0x80) return chr({});
                return chr(((c0 & 0x0Fu) << 12) |
                           ((c1 & 0x3Fu) << 6) |
                           ((c2 & 0x3Fu) << 0));
            }
            if ((c0 & 0xF8) == 0xF0) {
                if (!hasData(3)) return done();
                auto c1 = take();
                if ((c1 & 0xC0) != 0x80) return chr({});
                auto c2 = take();
                if ((c2 & 0xC0) != 0x80) return chr({});
                auto c3 = take();
                if ((c3 & 0xC0) != 0x80) return chr({});
                return chr(((c0 & 0x07u) << 18) |
                           ((c1 & 0x3Fu) << 12) |
                           ((c2 & 0x3Fu) << 6) |
                           ((c3 & 0x3Fu) << 0));
            }
            return chr({});
        }

        MemoryView data;
    };
    return Utf8Decoder{data};
}

#define TAB_WIDTH 4u

auto scan(const FileContent &file,
          Generator<FileChar> input) -> Generator<Token> {
    struct Scanner {
        Scanner(const FileContent &file_,
                Generator<FileChar> input_)
            : file(file_)
            , input(std::move(input_)) {
            if (input) fc = input.next();
        }

        operator bool() const { return fc.codePoint != 0; }
        auto next() -> Token {
            cursor.ptr = fc.ptr;
            auto next = [&] {
                if (input)
                    return input.next();
                else
                    return FileChar{};
            };
            auto token = [begin = cursor,
                          this](auto &&variant) {
                return Token{
                    TextRange{&file, begin, cursor},
                    std::forward<decltype(variant)>(
                        variant)};
            };

            auto &cp = fc.codePoint;
            auto isNewLine = [&] {
                return cp == '\n' || cp == '\r';
            };
            if (isNewLine()) {
                cursor.row++;
                cursor.column = {};
                auto tmp = cp;
                fc = next();
                if (fc.codePoint != tmp && isNewLine())
                    fc = next();
                return token(NewLine{});
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
                return token(WhiteSpace{});
            }

            auto single = [&](auto v) {
                cursor.column++;
                fc = next();
                return token(std::move(v));
            };
            switch (cp) {
            case ':': return single(Colon{});
            case ',': return single(Comma{});
            case '(': return single(ParenOpen{});
            case ')': return single(ParenClose{});
            case '=': return single(Equals{});
            case '+': return single(Plus{});
            case '-': return single(Minus{});
            case '*': return single(Multiply{});
            case '/': return single(Divide{});
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
                return token(Number{value});
            }

            auto isIdentChar = [&] {
                return (cp >= 'a' && cp <= 'z') ||
                       (cp >= 'A' && cp <= 'Z') ||
                       cp == '_';
            };
            if (isIdentChar()) {
                auto b = fc.ptr;
                do {
                    cursor.column++;
                    fc = next();
                } while (isIdentChar() || isDigit());
                auto intern = b; // TODO
                return token(Ident{intern});
            }
            return {};
        }

    private:
        FileContent file;
        Generator<FileChar> input;
        TextPosition cursor{};
        FileChar fc{};
    };

    return {Scanner{file, std::move(input)}};
}
