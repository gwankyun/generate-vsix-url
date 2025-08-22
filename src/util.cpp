module;
#include <nlohmann/json.hpp>
#define NOMINMAX
#include <windows.h>

module util;
import std;

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace util
{
    json load_json_file(const std::string& filename)
    {
        try
        {
            std::ifstream ifs(filename);
            if (!ifs.is_open())
            {
                throw std::ios_base::failure("无法打开文件: " + filename);
            }
            return json::parse(ifs);
        }
        catch (const json::parse_error& e)
        {
            // std::cerr << "JSON 解析错误: " << e.what() << " (行: " << e.line() << ", 列: " << e.column() << ")"
            //           << std::endl;
            throw;
        }
        catch (const std::exception& e)
        {
            std::cerr << "读取失败: " << e.what() << std::endl;
            throw;
        }
    }

    std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;

        auto to_str = [](auto&& part) { return std::string(part.begin(), part.end()); };

        auto parts =                       //
            s                              //
            | std::views::split(delimiter) //
            | std::views::transform(to_str);
        std::ranges::copy(parts, std::back_inserter(tokens));
        return tokens;
    }

    std::string executable_path()
    {
        char path[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, path, sizeof(path));
        std::string p(path);
        return p.substr(0, p.find_last_of('\\'));
    }
} // namespace util
