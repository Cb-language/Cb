#include "LSPPacker.h"
#include <sstream>
#include <map>
#include "other/Utils.h"

std::string LSPPacker::pack(const std::vector<Error*>& errors)
{
    std::ostringstream oss;

    // Group errors by file
    std::map<std::string, std::vector<Error*>> errorsByFile;
    for (Error* error : errors) {
        errorsByFile[error->getToken().path.string()].push_back(error);
    }

    oss << "[\n";

    bool firstFile = true;
    for (const auto& pair : errorsByFile) {
        if (!firstFile) {
            oss << ",\n";
        }
        firstFile = false;

        oss << "  {\n";
        oss << "    \"jsonrpc\": \"2.0\",\n";
        oss << "    \"method\": \"textDocument/publishDiagnostics\",\n";
        oss << "    \"params\": {\n";
        oss << "      \"uri\": \"file://" << Utils::escapeJson(Utils::normalizePath(pair.first)) << "\",\n";
        oss << "      \"diagnostics\": [\n";

        bool first = true;
        for (Error* error : pair.second) {
            if (!first) {
                oss << ",\n";
            }
            first = false;

            const Token& token = error->getToken();
            int line = token.line > 0 ? token.line - 1 : 0; // LSP is 0-indexed
            int col = token.column > 0 ? token.column - 1 : 0; // LSP is 0-indexed

            oss << "        {\n";
            oss << "          \"range\": {\n";
            oss << "            \"start\": { \"line\": " << line << ", \"character\": " << col << " },\n";
            oss << "            \"end\": { \"line\": " << line << ", \"character\": " << col + token.value.length() << " }\n";
            oss << "          },\n";
            oss << "          \"severity\": 1,\n";
            oss << "          \"source\": \"transpiler\",\n";
            oss << "          \"message\": \"" << Utils::escapeJson(error->getErrorMessage()) << "\"\n";
            oss << "        }";
        }
        oss << "\n      ]\n";
        oss << "    }\n";
        oss << "  }";
    }

    oss << "\n]\n";

    return oss.str();
}
