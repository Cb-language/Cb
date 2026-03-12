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
    if (c.matchNonConsume(CbTokenType::TYPE_RIFF))
    {
        return parseArrayType();
    } // if not array try primitive types

    return parseType();
}

std::unique_ptr<IType> TypeParser::parseType() const
{
    if (c.matchNonConsume(CbTokenType::IDENTIFIER))
    {
        return std::make_unique<ClassType>(c.parseFQN());
    }

    auto signType = SignType::SIGNED;

    if (c.matchConsume(CbTokenType::TYPE_FLAT))
    {
    }

    else if (c.matchConsume(CbTokenType::TYPE_SHARP))
    {
        signType = SignType::UNSIGNED;
    }

    Token typeToken;
    if (!c.expect(CbTokenType::TYPE_DEGREE, nullptr, typeToken) &&
        !c.expect(CbTokenType::TYPE_FREQ, nullptr, typeToken) &&
        !c.expect(CbTokenType::TYPE_NOTE, nullptr, typeToken) &&
        !c.expect(CbTokenType::TYPE_MUTE, nullptr, typeToken) &&
        !c.expect(CbTokenType::TYPE_BAR, nullptr, typeToken) &&
        !c.expect(CbTokenType::TYPE_FERMATA, nullptr, typeToken))
    {
        c.addError(std::make_unique<InvalidNumberLiteral>(c.current()));
        return nullptr;
    }

    Primitive value;
    switch (typeToken.type)
    {
        case CbTokenType::TYPE_DEGREE:  value = Primitive::TYPE_DEGREE;  break;
        case CbTokenType::TYPE_FREQ:    value = Primitive::TYPE_FREQ;    break;
        case CbTokenType::TYPE_NOTE:    value = Primitive::TYPE_NOTE;    break;
        case CbTokenType::TYPE_MUTE:    value = Primitive::TYPE_MUTE;    break;
        case CbTokenType::TYPE_BAR:     value = Primitive::TYPE_BAR;     break;
        case CbTokenType::TYPE_FERMATA: value = Primitive::TYPE_FERMATA; break;

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
    c.matchConsume(CbTokenType::TYPE_RIFF);

    std::unique_ptr<IType> arrType = parseType();
    if (!arrType)
        return nullptr;

    return std::make_unique<ArrayType>(std::move(arrType));
}
