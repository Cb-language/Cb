#include <iostream>
#include <locale>
#include "Tokenizer.h"

void test()
{
    std::wstring code = LR"(
? This is a single-line comment
?* This is a 
multi-line comment *?
cres demen 3.14 42 'a' "hello\nworld"
flat degree playBar hear D Fmin
♯ ♭ ♮
x += 5
y == 10
z // 2
[𝄞] ║ \ identifier_name
)";

    std::vector<Token> tokens = Tokenizer::tokenize(code);

    for (const auto& tok : tokens)
    {
        std::wcout << L"Type: " << tok.type
            << L", Value: \"" << tok.value << L"\""
            << L", Line: " << tok.line
            << L", Column: " << tok.column
            << std::endl;
    }
}

int main()
{
    // Set locale for wide character output
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());

    Tokenizer::init();

    test();

    return 0;
}
