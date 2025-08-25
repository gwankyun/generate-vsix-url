module;

export module control;
import std;

export namespace control
{
    bool InputText(const std::string& _label, std::vector<char>& _buffer);

    void Text(const std::string& _label, std::size_t _size);
} // namespace control
