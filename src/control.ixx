module;
#include <hello_imgui/hello_imgui.h>

export module control;
import std;

export namespace control
{
    bool InputText(const std::string& _label, std::vector<char>& _buffer)
    {
        return ImGui::InputText(_label.c_str(), _buffer.data(), _buffer.size());
    }

    void Text(const std::string& _label, std::size_t _size)
    {
        std::vector<char> buffer(_size, ' ');
        buffer[_size - 1] = '\0';
        std::copy_n(_label.c_str(), std::min(_label.size(), buffer.size() - 1), buffer.data());
        ImGui::Text(buffer.data());
    }
} // namespace control
