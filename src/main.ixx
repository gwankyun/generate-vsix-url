module;

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <boost/algorithm/string.hpp>
#include <boost/scope/defer.hpp >
#include <clip.h>
#include <hello_imgui/hello_imgui.h>
#include <nlohmann/json.hpp>

export module main;
import std;
import control;
import util;

using json = nlohmann::json;
namespace fs = std::filesystem;

struct Item
{
    std::string id = "";
    std::string version = "";
    std::string platform = "";
};

bool listbox_getter(void* data, int index, const char** output)
{
    auto waypoints = (std::vector<std::string>*)data;
    *output = (*waypoints)[index].c_str();
    return true;
}

struct Extension
{
    // std::string id;
    std::string version;
    std::string platform;
};

struct Config
{
    std::filesystem::path current;
    std::string platform;
    std::map<std::string, Extension> map_items;
    std::vector<std::string> listbox_items;
    int listbox_item_current = 1;
    struct Text
    {
        std::string version = "version";
        std::string platform = "platform";
    };
    Text text;
};

void save(Config& config, fs::path data_path)
{
    SPDLOG_INFO("current: {}", config.current.string());

    json j;
    for (auto& i : config.listbox_items)
    {
        auto value = config.map_items[i];
        auto& text = config.text;
        j[i] = {{config.text.version, value.version}, {config.text.platform, value.platform}};
    }

    std::ofstream outFile(data_path.string());
    if (outFile.is_open())
    {
        outFile << j.dump(4) << std::endl;
    }
}

void generate(const std::string& id, const Extension& value)
{
    std::string base_url = "https://marketplace.visualstudio.com/_apis/public/gallery";
    auto vec = util::split(id, '.');
    auto publisher = vec[0];
    auto name = vec[1];
    SPDLOG_INFO("publisher: {}", publisher);
    SPDLOG_INFO("name: {}", name);
    auto download_url =
        fmt::format("{}/publishers/{}/vsextensions/{}/{}/vspackage", base_url, publisher, name, value.version);
    if (!value.platform.empty())
    {
        download_url += fmt::format("?targetPlatform={}", value.platform);
    }
    clip::set_text(download_url);
}

struct Add
{
    std::vector<char> id;
    std::vector<char> version;
    std::vector<char> platform;
    Item addItem;
    void clear()
    {
        id.resize(128, '\0');
        std::fill_n(id.begin(), id.size(), '\0');
        version.resize(128, '\0');
        std::fill_n(version.begin(), version.size(), '\0');
        platform.resize(128, '\0');
        std::fill_n(platform.begin(), platform.size(), '\0');
    }
};

void add_pop(Config& config, Add& add)
{
    auto id = "OpenPopup_add";
    auto input_identifier = "##InputText_add_identifier";
    auto input_version = "##InputText_add_version";
    auto input_platform = "##InputText_add_platform";

    // 触发条件（如按钮点击）
    if (ImGui::Button("add"))
    {
        ImGui::OpenPopup(id); // 弹窗ID需唯一
    }

    // 弹窗内容
    if (ImGui::BeginPopup(id))
    {
        BOOST_SCOPE_DEFER[]
        {
            ImGui::EndPopup();
        };

        ImGui::Text("add");
        ImGui::Separator();

        // add.clear();
        auto text_len = 15;

        control::Text("identifier: ", text_len);
        ImGui::SameLine();
        if (control::InputText(input_identifier, add.id))
        {
            add.addItem.id = add.id.data();
            boost::algorithm::trim(add.addItem.id);
        }

        // ImGui::Text("version: ");
        control::Text("version: ", text_len);
        ImGui::SameLine();
        if (control::InputText(input_version, add.version))
        {
            add.addItem.version = add.version.data();
            boost::algorithm::trim(add.addItem.version);
        }

        // ImGui::Text("platform: ");
        control::Text("platform: ", text_len);
        ImGui::SameLine();
        if (control::InputText(input_platform, add.platform))
        {
            add.addItem.platform = add.platform.data();
            boost::algorithm::trim(add.addItem.platform);
        }

        if (ImGui::Button("ok"))
        {
            config.map_items[add.addItem.id] = Extension{add.addItem.version, add.addItem.platform};

            ImGui::CloseCurrentPopup(); // 关闭弹窗
        }

        ImGui::SameLine();

        if (ImGui::Button("cancel"))
        {
            ImGui::CloseCurrentPopup(); // 关闭弹窗
        }
    }
}

struct Update
{
    std::vector<char> key;
    std::vector<char> value;
    Item updateItem;
    void clear()
    {
        key.resize(128, '\0');
        std::fill_n(key.begin(), key.size(), '\0');
        value.resize(128, '\0');
        std::fill_n(value.begin(), value.size(), '\0');
    }
};

void update_pop(Config& config, Update& update, const std::string& _current)
{
    auto pop_name = "OpenPopup_update";
    auto input_version = "##InputText_update_version";
    auto input_platform = "##InputText_update_platform";
    if (ImGui::Button("update"))
    {
        ImGui::OpenPopup(pop_name); // 弹窗ID需唯一
    }

    if (ImGui::BeginPopup(pop_name))
    {
        BOOST_SCOPE_DEFER[]
        {
            ImGui::EndPopup();
        };

        ImGui::Text("update");
        ImGui::Separator();

        update.clear();

        auto key = update.updateItem.version;
        std::copy_n(key.c_str(), key.size(), update.key.begin());

        if (control::InputText(input_version, update.key))
        {
            update.updateItem.version = update.key.data();
            boost::algorithm::trim(update.updateItem.version);
        }

        auto value = update.updateItem.platform;
        std::copy_n(value.c_str(), value.size(), update.value.begin());

        if (control::InputText(input_platform, update.value))
        {
            update.updateItem.platform = update.value.data();
            boost::algorithm::trim(update.updateItem.platform);
        }

        if (ImGui::Button("ok"))
        {
            config.map_items[_current] = Extension{update.updateItem.version, update.updateItem.platform};

            ImGui::CloseCurrentPopup(); // 关闭弹窗
        }

        ImGui::SameLine();

        if (ImGui::Button("cancel"))
        {
            ImGui::CloseCurrentPopup(); // 关闭弹窗
        }
    }
}

export int main(int _argc, char* _argv[])
{
    spdlog::set_pattern("[%C-%m-%d %T.%e] [%^%L%$] [t:%6t] [%-8!!:%4#] %v");

    auto exe_path = fs::path(util::executable_path());
    SPDLOG_INFO("executable_path: {}", exe_path.string());

    util::hide_window();

    Add add;
    add.clear();

    Config config;

    config.current = fs::current_path();

    // 自動加載
    auto data_path = exe_path;

    while (true)
    {
        auto config_path = data_path / "config";
        if (fs::exists(config_path))
        {
            auto json_path = config_path / "save.json";
            data_path = json_path;
            break;
        }
        data_path = data_path.parent_path();
    }

    if (fs::exists(data_path))
    {
        json data = util::load_json_file(data_path.string());

        for (auto& [key, value] : data.items())
        {
            // std::cout << key << ": " << value << std::endl;
            SPDLOG_INFO("key: {} value: {}", key, value.dump());
            config.map_items[key] = Extension{value[config.text.version], value[config.text.platform]};
        }
    }
    else
    {
        config.map_items["marscode.marscode-extension"] = Extension{"1.2.44", ""};
        config.map_items["rust-lang.rust-analyzer"] = Extension{"0.3.2577", "win32-x64"};
    }

    Update update;
    update.clear();

    HelloImGui::Run(
        [&]
        {
            config.listbox_items.clear();
            for (auto i : config.map_items)
            {
                config.listbox_items.push_back(i.first);
            }
            // std::sort(config.listbox_items.begin(), config.listbox_items.end());
            std::ranges::sort(config.listbox_items);

            if (std::filesystem::exists(data_path))
            {
                ImGui::Text(data_path.string().c_str());
            }

            if (ImGui::ListBox("##listbox::Cavebot", &config.listbox_item_current, listbox_getter,
                               &config.listbox_items, config.listbox_items.size()))
            {
                SPDLOG_INFO("listbox_item_current: {}", config.listbox_item_current);
                auto key = config.listbox_items[config.listbox_item_current];
                auto value = config.map_items[key];
                SPDLOG_INFO("key: {}", key);
                // SPDLOG_INFO("value: {}", value);
                update.updateItem.version = value.version;
                update.updateItem.platform = value.platform;
                // updateItem.value = value;
            }

            if (config.listbox_item_current < config.listbox_items.size())
            {
                auto id = config.listbox_items[config.listbox_item_current];
                auto value = config.map_items[id];
                auto text_len = 10;

                auto show_text = [&](const std::string& _label, const std::string& _str)
                {
                    control::Text(_label, text_len);
                    ImGui::SameLine();
                    ImGui::Text(_str.c_str());
                };

                show_text("id: ", id);

                show_text("version ", value.version);

                show_text("platform ", value.platform);

                if (!value.platform.empty())
                {
                    ImGui::SameLine();
                    if (ImGui::Button("copy"))
                    {
                        clip::set_text(value.platform);
                    }
                }

                if (ImGui::Button("remove"))
                {
                    config.map_items.erase(id);
                }

                ImGui::SameLine();

                if (ImGui::Button("generate"))
                {
                    generate(id, value);
                }

                ImGui::SameLine();

                add_pop(config, add);

                ImGui::SameLine();

                update_pop(config, update, id);

                ImGui::SameLine();
            }

            // 寫入文件
            if (ImGui::Button("save"))
            {
                save(config, data_path);
            }
        },                          // Gui code
        "generate vsix url", true); // Window title + Window size auto
    return 0;
}
