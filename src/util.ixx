module;
#include <nlohmann/json.hpp>

export module util;
import std;

using json = nlohmann::json;
namespace fs = std::filesystem;

export namespace util
{
    json load_json_file(const std::string& filename);

    std::vector<std::string> split(const std::string& s, char delimiter);

    std::string executable_path();
} // namespace util
