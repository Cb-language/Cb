#include <string>
#include <memory>

class CMD
{
public:
    virtual ~CMD() = default;

    virtual void setupConsole() = 0;

    virtual std::string getNullDevice() const = 0;

    virtual std::string getExeExtension() const = 0;

    virtual std::string getCompileFlags() const = 0;

    virtual void runExecutable(const std::string& path) const = 0;
};

std::unique_ptr<CMD> createCMD();