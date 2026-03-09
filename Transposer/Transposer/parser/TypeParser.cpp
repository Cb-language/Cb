#include "TypeParser.h"
#include "symbols/Type/Type.h"
#include "symbols/Type/ArrayType.h"
#include "symbols/Type/ClassType.h"
#include "errorHandling/lexicalErrors/UnrecognizedToken.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/lexicalErrors/InvalidNumberLiteral.h"

TypeParser::TypeParser(ParserContext& c) : c(c)
{
}

void TypeParser::parse() const
{
    // This sub-parser might not have a standalone parse() if it's always called by others
}

std::unique_ptr<IType> TypeParser::parseIType()
{
    if (c.match(TokenType::TYPE_RIFF))
    {
        c.advance();
        return parseArrayType();
    } // if not array try primitive types

    if (c.isType()) return parseType();

    return parseClassType(); // if not array and not primitive parse class
}

std::unique_ptr<IType> TypeParser::parseType() const
{
    std::string prefix = "";
    if (c.match(TokenType::TYPE_FLAT))
    {
        prefix = "flat ";
        c.advance();
    }
    else if (c.match(TokenType::TYPE_SHARP))
    {
        prefix = "sharp ";
        c.advance();
    }

    Token typeToken;
    if (!c.expect(TokenType::TYPE_DEGREE, nullptr, typeToken) &&
        !c.expect(TokenType::TYPE_FREQ, nullptr, typeToken) &&
        !c.expect(TokenType::TYPE_NOTE, nullptr, typeToken) &&
        !c.expect(TokenType::TYPE_MUTE, nullptr, typeToken) &&
        !c.expect(TokenType::TYPE_BAR, nullptr, typeToken) &&
        !c.expect(TokenType::TYPE_FERMATA, nullptr, typeToken))
    {
        c.addError(std::make_unique<InvalidNumberLiteral>(c.current()));
        return nullptr;
    }

    std::string value = "";
    switch (typeToken.type)
    {
        case TokenType::TYPE_DEGREE: value = "degree"; break;
        case TokenType::TYPE_FREQ:   value = "freq";   break;
        case TokenType::TYPE_NOTE:   value = "note";   break;
        case TokenType::TYPE_MUTE:   value = "mute";   break;
        case TokenType::TYPE_BAR:    value = "bar";    break;
        case TokenType::TYPE_FERMATA:value = "fermata";break;
        default: break;
    }

    if (!prefix.empty() && value == "freq")
    {
        c.addError(std::make_unique<UnrecognizedToken>(typeToken));
        return nullptr;
    }

    return std::make_unique<Type>(prefix + value);
}

std::unique_ptr<IType> TypeParser::parseArrayType()
{
    std::unique_ptr<IType> arrType = parseIType();
    if (!arrType) return nullptr;

    return std::make_unique<ArrayType>(std::move(arrType));
}

std::unique_ptr<IType> TypeParser::parseClassType() const
{
    const std::string classname = c.current().value.value_or("");
    Token classToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingBrace>(c.current()), classToken)) return nullptr;

    if (auto cls = c.getSymTable().getClass(classname))
    {
        return std::make_unique<ClassType>(cls);
    }

    return nullptr;
}
