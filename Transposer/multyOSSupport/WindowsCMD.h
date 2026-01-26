#include <windows.h>
#include "CMD.h"

class WindowsCMD : public CMD
{
public:
    void setupConsole() override;

    std::string getNullDevice() const override;

    std::string getExeExtension() const override;

    std::string getCompileFlags() const override;

    void runExecutable(const std::string& path) const override;
};

std::unique_ptr<CMD> createCMD();
