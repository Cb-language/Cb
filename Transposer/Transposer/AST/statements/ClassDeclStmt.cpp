#include "ClassDeclStmt.h"

#include <ranges>

#include "FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"
#include "errorHandling/classErrors/InvalidOverrideSignature.h"
#include "errorHandling/classErrors/OverrideError.h"

std::string ClassDeclStmt::generateToString() const
{
    std::ostringstream oss;
    const std::string className = Utils::wstrToStr(name);

    // Function signature
    oss << "std::string " << className << "::toString(int indents) const" << std::endl;
    oss << "{" << std::endl;

    // Start of class string
    oss << "\tstd::string str = \"" << className << " {\\n\";" << std::endl;

    // Loop over fields
    for (auto& field : fields | std::views::values)
    {
        const std::string fieldName = Utils::wstrToStr(field->getVar().getName());
        oss << "\tstr += getIndents(indents + 1) + \"" << fieldName << " = ";
        if (field->getVar().isPrimitive())
            oss << "\" + " << fieldName << ".toString() + \"\\n\";" << std::endl;
        else
            oss << "\" + " << fieldName << ".toString(indents + 1) + \"\\n\";" << std::endl;
    }

    // Include parent class toString if applicable
    if (const auto parent = currClass->getParent())
    {
        if (parent->getClass().getClassName() != L"Object")
        {
            oss << "\tstr += getIndents(indents + 1) + " << Utils::wstrToStr(parent->getClass().getClassName())
                << "::toString(indents + 1) + \"\\n\";" << std::endl;
        }
    }

    // Close class braces
    oss << "\tstr += getIndents(indents) + \"}\";" << std::endl;
    oss << "\treturn str;" << std::endl;
    oss << "}" << std::endl;

    return oss.str();
}

std::string ClassDeclStmt::generateEquals() const
{
    std::ostringstream oss;
    const std::string strName = Utils::wstrToStr(name);
    oss << "Primitive<bool> " << strName << "::equals(const Object& other) const" << std::endl <<
    "{" << std::endl <<
        "\tif (this == &other) return Primitive<bool>(true);" << std::endl <<
        std::endl <<
        "\tif (const " << strName << "* otherPtr = dynamic_cast<const " << strName << "*>(&other))" << std::endl <<
        "\t{" << std::endl <<
            "\t\tif (";

    if (auto parent = currClass->getParent()) oss << Utils::wstrToStr(parent->getClass().getClassName()) << "::equals(other)";
    else oss << "true"; // shouldn't get here tho

    for (const auto& field : fields | std::views::values)
    {
        const std::string fieldName = Utils::wstrToStr(field->getVar().getName());
        oss << " && ";
        oss << fieldName << " == otherPtr->" << fieldName;
    }
    oss << ") return Primitive<bool>(true);" << std::endl <<
        "\t}" << std::endl <<
        "\treturn Primitive<bool>(false);" << std::endl <<
        "}" << std::endl;

    return oss.str();
}

ClassDeclStmt::ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                             const std::wstring& name, std::vector<Field>& fields, std::vector<Method>& methods, std::vector<Ctor>& ctors,
                             const bool isInheriting, const std::wstring& inheritingPublic, const std::wstring& inheritingName)
    : Stmt(token, scope, funcDecl, currClass), name(name), isInheriting(isInheriting), inheritingPublic(inheritingPublic), inheritingName(inheritingName)
{
    for (auto& [isPublic, func] : fields) this->fields.emplace_back(isPublic, std::move(func));
    for (auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, std::move(method));
    for (auto& [isPublic, ctor] : ctors)
    {
        if (!hasEmptyCtor && ctor->getConstractor().getArgs().empty()) hasEmptyCtor = true;
        this->ctors.emplace_back(isPublic, std::move(ctor));
    }

    if (!hasEmptyCtor)
    {
        std::vector<Var> emptyArgs;
        std::vector<std::unique_ptr<Stmt>> emptyBodyStmts;
        auto ctor = std::make_unique<ConstractorDeclStmt>(
                token,
                scope,
                currClass,
                name,
                std::move(emptyArgs)
            );
        ctor->setBody(std::make_unique<BodyStmt>(
                token,
                scope,
                nullptr,
                currClass,
                emptyBodyStmts
            )
        );

        this->ctors.emplace_back(PUBLIC, std::move(ctor));
    }
}

void ClassDeclStmt::analyze() const
{
    for (const auto& field : fields | std::views::values) field->analyze();
    for (const auto& ctor : ctors | std::views::values) ctor->analyze();

    bool isAbstract = false;

    for (const auto& method : methods | std::views::values)
    {
        method->analyze();
        if (method->getVirtual() == VirtualType::PURE)
        {
            isAbstract = true;
        }
        else if (method->getVirtual() == VirtualType::OVERRIDE)
        {
            const ClassNode* parent = this->currClass->getParent();
            if (parent == nullptr || parent->getClass().getClassName() == L"Object")
            {
                throw OverrideError(token);
            }

            const Func* baseMethod = parent->findMethod(method->getName());
            if (baseMethod == nullptr)
            {
                throw OverrideError(token);
            }

            if (baseMethod->getVirtual() != VirtualType::VIRTUAL && baseMethod->getVirtual() != VirtualType::PURE)
            {
                throw OverrideError(token);
            }

            if (!baseMethod->isSameNameAndArgs(method->getFunc()))
            {
                throw InvalidOverrideSignature(token);
            }
        }
    }

    if (const ClassNode* parent = this->currClass->getParent())
    {
        if (parent->isAbstract())
        {
            for (const auto& baseMethod : parent->getClass().getMethods() | std::views::values)
            {
                if (baseMethod.getVirtual() == VirtualType::PURE)
                {
                    bool implemented = false;
                    for (const auto& method : methods | std::views::values)
                    {
                        if (method->getFunc().isSameNameAndArgs(baseMethod))
                        {
                            if (method->getVirtual() != VirtualType::PURE)
                            {
                                implemented = true;
                            }
                            break;
                        }
                    }

                    if (!implemented)
                    {
                        isAbstract = true;
                    }
                }
            }
        }
    }

    const_cast<ClassNode*>(this->currClass)->setAbstract(isAbstract);


    if (isInheriting)
    {
        if (inheritingName.empty())
        {
            throw HowDidYouGetHere(token);
        }
        if (inheritingPublic != L"tutti" && inheritingPublic != L"section" && inheritingPublic != L"sordino")
        {
            throw HowDidYouGetHere(token);
        }
    }
}

std::string ClassDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    for (const auto& ctor : ctors | std::views::values) oss << Utils::removeAllFirstTabs(ctor->translateToCpp()) << std::endl;

    oss << generateEquals() << std::endl;
    oss << generateToString() << std::endl;

    for (const auto& method : methods | std::views::values) oss << Utils::removeAllFirstTabs(method->translateToCppClass(name)) << std::endl;
    return oss.str();
}

std::string ClassDeclStmt::translateToH() const
{
    std::ostringstream oss;
    std::ostringstream privates;
    std::ostringstream publics;
    std::ostringstream protecteds;
    const std::string tabs = getTabs();

    privates << "private:";
    publics << "public:";
    protecteds << "protected: ";

    oss << "class " << Utils::wstrToStr(name);
    if (isInheriting)
    {
        oss << " : ";
        if (inheritingPublic == L"tutti")
        {
            oss << "public ";
        }
        else if (inheritingPublic == L"section")
        {
            oss << "protected ";
        }
        else
        {
            oss << "private ";
        }

        oss << Utils::wstrToStr(inheritingName);
    }
    else
    {
        oss << " : public Object";
    }
    oss << std::endl << "{" << std::endl;

    for (const auto& [isPublic, field] : fields)
    {
        if (isPublic == PUBLIC) publics << std::endl << tabs << Utils::removeAllFirstTabs(field->translateToCpp());
        else if (isPublic == PROTECTED) protecteds << std::endl << tabs << Utils::removeAllFirstTabs(field->translateToCpp());
        else privates << std::endl << tabs << Utils::removeAllFirstTabs(field->translateToCpp());
    }

    for (const auto& [isPublic, method] : methods)
    {
        if (isPublic == PUBLIC) publics << std::endl << tabs << Utils::removeAllFirstTabs(method->translateToH());
        else if (isPublic == PROTECTED) protecteds << std::endl << tabs << Utils::removeAllFirstTabs(method->translateToH());
        else privates << std::endl << tabs << Utils::removeAllFirstTabs(method->translateToH());
    }

    for (const auto& [isPublic, ctor] : ctors)
    {
        if (isPublic == PUBLIC) publics << std::endl << tabs << Utils::removeAllFirstTabs(ctor->translateToH());
        else if (isPublic == PROTECTED) protecteds << std::endl << tabs << Utils::removeAllFirstTabs(ctor->translateToH());
        else privates << std::endl << tabs << Utils::removeAllFirstTabs(ctor->translateToH());
    }

    publics << std::endl << tabs << "std::string toString(int indents = 0) const override;" << std::endl << tabs << "Primitive<bool> equals(const Object& other) const override;" << std::endl;

    oss << privates.str() << std::endl << std::endl << publics.str() << std::endl << protecteds.str() << std::endl << "};" << std::endl << std::endl;
    return oss.str();
}

bool ClassDeclStmt::getHasEmptyCtor() const
{
    return hasEmptyCtor;
}
