#include "ClassDeclStmt.h"

#include <ranges>

#include "FuncDeclStmt.h"
#include "errorHandling/classErrors/ClassDosentExisit.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"
#include "errorHandling/classErrors/InvalidOverrideSignature.h"
#include "errorHandling/classErrors/NoOverrideError.h"
#include "errorHandling/classErrors/UnimplementedPureVirtualMethod.h"

std::string ClassDeclStmt::generateToString() const
{
    std::ostringstream oss;
    const std::string className = translateFQNtoString(name);

    // Function signature
    oss << "std::string " << className << "::toString(int indents) const" << std::endl;
    oss << "{" << std::endl;

    // Start of class string
    oss << "\tstd::string str = \"" << className << " {\\n\";" << std::endl;

    // Loop over fields
    for (auto& field : fields | std::views::values)
    {
        if (field->getVar().getStatic()) continue;
        const std::string fieldName = translateFQNtoString(field->getVar().getName());
        oss << "\tstr += getIndents(indents + 1) + \"" << fieldName << " = ";
        if (field->getVar().isPrimitive())
            oss << "\" + " << fieldName << ".toString() + \"\\n\";" << std::endl;
        else
            oss << "\" + " << fieldName << ".toString(indents + 1) + \"\\n\";" << std::endl;
    }

    // Include parent class toString if applicable
    if (const auto parent = currClass->getParent())
    {
        if (translateFQNtoString(parent->getClass().getClassName()) != "Object")
        {
            oss << "\tstr += getIndents(indents + 1) + " << translateFQNtoString(parent->getClass().getClassName())
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
    const std::string strName = translateFQNtoString(name);
    oss << "Primitive<bool> " << strName << "::equals(const Object& other) const" << std::endl <<
    "{" << std::endl <<
        "\tif (this == &other) return Primitive<bool>(true);" << std::endl <<
        std::endl <<
        "\tif (const " << strName << "* otherPtr = dynamic_cast<const " << strName << "*>(&other))" << std::endl <<
        "\t{" << std::endl <<
            "\t\tif (";

    if (const auto parent = currClass->getParent()) oss << translateFQNtoString(parent->getClass().getClassName()) << "::equals(other)";
    else oss << "true"; // shouldn't get here tho

    for (const auto& field : fields | std::views::values)
    {
        if (field->getVar().getStatic()) continue;
        const std::string fieldName = translateFQNtoString(field->getVar().getName());
        oss << " && ";
        oss << fieldName << " == otherPtr->" << fieldName;
    }
    oss << ") return Primitive<bool>(true);" << std::endl;
    oss << "\t}" << std::endl;
    oss << "\treturn Primitive<bool>(false);" << std::endl;
    oss << "}" << std::endl;

    return oss.str();
}

ClassDeclStmt::ClassDeclStmt(const Token& token, const FQN& name, std::vector<Field>& fields,
    std::vector<Method>& methods, std::vector<Ctor>& ctors,
    const FQN& parentName)
    : Stmt(token), name(name), parentName(parentName)
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
        auto ctor = std::make_unique<ConstructorDeclStmt>(
                token,
                name,
                std::move(emptyArgs)
            );
        ctor->setBody(std::make_unique<BodyStmt>(
                token,
                emptyBodyStmts
            )
        );

        this->ctors.emplace_back(PUBLIC, std::move(ctor));
    }
}
void ClassDeclStmt::analyze() const
{
    if (symTable == nullptr) return;

    if (!parentName.empty())
    {
        if (SymbolTable::getClass(parentName) == nullptr)
        {
            throw ClassDosentExisit(token, translateFQNtoString(parentName));
        }
    }

    for (const auto& field : fields | std::views::values)
    {
        field->setSymbolTable(symTable);
        field->setScope(scope);
        field->setClassNode(currClass);
        field->analyze();
    }
    for (const auto& ctor : ctors | std::views::values)
    {
        ctor->setSymbolTable(symTable);
        ctor->setScope(scope);
        ctor->setClassNode(currClass);
        ctor->analyze();
    }

    bool isAbstract = false;

    for (const auto& method : methods | std::views::values)
    {
        method->setSymbolTable(symTable);
        method->setScope(scope);
        method->setClassNode(currClass);
        method->analyze();

        switch (method->getVirtual())
        {
        case VirtualType::PURE:
        {
            isAbstract = true;
            break;
        }

        case VirtualType::OVERRIDE:
        {
            const ClassNode* parent = this->currClass->getParent();
            if (parent == nullptr || translateFQNtoString(parent->getClass().getClassName()) == "Object")
            {
                throw NoOverrideError(token);
            }

            const Func* baseMethod = parent->findMethod(method->getName());
            if (baseMethod == nullptr)
            {
                throw NoOverrideError(token);
            }

            if (baseMethod->getVirtual() != VirtualType::VIRTUAL && baseMethod->getVirtual() != VirtualType::PURE)
            {
                throw NoOverrideError(token);
            }

            if (!baseMethod->isSameNameAndArgs(method->getFunc()))
            {
                throw InvalidOverrideSignature(token);
            }
            break;
        }

        case VirtualType::NONE:
        {
            if (const auto parent = currClass->getParent())
            {
                if (const auto baseMethod = parent->findMethod(method->getName()))
                {
                    if (baseMethod->getVirtual() != VirtualType::NONE)
                    {
                        throw NoOverrideError(token);
                    }
                }
            }
        }

        default: break;
        }
    }

    if (const ClassNode* parent = this->currClass->getParent())
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
                    if (!isAbstract)
                    {
                        throw UnimplementedPureVirtualMethod(token, translateFQNtoString(baseMethod.getFuncName()));
                    }
                }
            }
        }
    }

    const_cast<ClassNode*>(this->currClass)->setAbstract(isAbstract);

}

std::string ClassDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    for (const auto& field : fields | std::views::values)
    {
        if (field->getVar().getStatic())
        {
            oss << field->getVar().getType()->translateTypeToCpp() << " " << translateFQNtoString(name) << "::" << translateFQNtoString(field->getVar().getName());
            if (const auto expr = field->getStartingValue())
            {
                oss << " = " << expr->translateToCpp();
            }
            oss << ";" << std::endl;
        }
    }

    for (const auto& ctor : ctors | std::views::values) oss << Utils::removeAllFirstTabs(ctor->translateToCpp()) << std::endl;

    oss << generateEquals() << std::endl;
    oss << generateToString() << std::endl;

    for (const auto& method : methods | std::views::values) oss << Utils::removeAllFirstTabs(method->translateToCppClass(translateFQNtoString(name))) << std::endl;
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

    oss << "class " << translateFQNtoString(name);
    if (!parentName.empty())
    {
        oss << " : public ";
        oss << translateFQNtoString(parentName);
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

void ClassDeclStmt::setClassDetails(std::vector<Field> fields, std::vector<Method> methods, std::vector<Ctor> ctors)
{
    this->fields = std::move(fields);
    this->methods = std::move(methods);
    this->ctors = std::move(ctors);
}

const FQN& ClassDeclStmt::getName() const
{
    return name;
}

const FQN& ClassDeclStmt::getParentName() const
{
    return parentName;
}

const std::vector<Field>& ClassDeclStmt::getFields() const
{
    return fields;
}

const std::vector<Method>& ClassDeclStmt::getMethods() const
{
    return methods;
}

const std::vector<Ctor>& ClassDeclStmt::getCtors() const
{
    return ctors;
}
