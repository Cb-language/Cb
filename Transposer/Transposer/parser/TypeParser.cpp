#include "TypeParser.h"
#include "symbols/Type/PrimitiveType.h"
#include "symbols/Type/ArrayType.h"
#include "symbols/Type/ClassType.h"
#include "errorHandling/lexicalErrors/UnrecognizedToken.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/lexicalErrors/InvalidNumberLiteral.h"

TypeParser::TypeParser(ParserContext& c) : c(c)
{
}

std::unique_ptr<IType> TypeParser::parseIType() const
{
    if (c.matchNonConsume(TokenType::TYPE_RIFF))
    {
        return parseArrayType();
    } // if not array try primitive types

    return parseType();
}

std::unique_ptr<IType> TypeParser::parseType() const
{
    if (Token name; c.matchConsume(TokenType::IDENTIFIER, name))
    {
        return std::make_unique<ClassType>(name.value.value());
    }

    auto signType = SignType::SIGNED;

    if (c.matchConsume(TokenType::TYPE_FLAT))
    {
    }

    else if (c.matchConsume(TokenType::TYPE_SHARP))
    {
        signType = SignType::UNSIGNED;
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

    Primitive value;
    switch (typeToken.type)
    {
        case TokenType::TYPE_DEGREE:  value = Primitive::TYPE_DEGREE;  break;
        case TokenType::TYPE_FREQ:    value = Primitive::TYPE_FREQ;    break;
        case TokenType::TYPE_NOTE:    value = Primitive::TYPE_NOTE;    break;
        case TokenType::TYPE_MUTE:    value = Primitive::TYPE_MUTE;    break;
        case TokenType::TYPE_BAR:     value = Primitive::TYPE_BAR;     break;
        case TokenType::TYPE_FERMATA: value = Primitive::TYPE_FERMATA; break;

        default: c.addError(std::make_unique<InvalidNumberLiteral>(c.current())); return nullptr;
    }

    if (signType == SignType::UNSIGNED && value == Primitive::TYPE_FREQ)
    {
        c.addError(std::make_unique<UnrecognizedToken>(typeToken));
        return nullptr;
    }

    return std::make_unique<PrimitiveType>(value, signType);
}

std::unique_ptr<IType> TypeParser::parseArrayType() const
{
    std::unique_ptr<IType> arrType = parseType();
    if (!arrType) return nullptr;

    return std::make_unique<ArrayType>(std::move(arrType));
}
