#include "FeatureBase.h"

#include "../GUI/imgui.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <locale>
#include <sstream>

namespace
{
    static std::string TrimCopy(const std::string& input)
    {
        size_t begin = 0;
        while (begin < input.size() && std::isspace(static_cast<unsigned char>(input[begin])))
            ++begin;

        size_t end = input.size();
        while (end > begin && std::isspace(static_cast<unsigned char>(input[end - 1])))
            --end;

        return input.substr(begin, end - begin);
    }

    static std::string NormalizeConfigKey(const std::string& key)
    {
        return TrimCopy(key);
    }

    static std::string ToLowerAscii(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return value;
    }

    static bool ParseBoolToken(const std::string& input, bool& out)
    {
        const std::string token = ToLowerAscii(TrimCopy(input));
        if (token == "1" || token == "true" || token == "on" || token == "yes")
        {
            out = true;
            return true;
        }
        if (token == "0" || token == "false" || token == "off" || token == "no")
        {
            out = false;
            return true;
        }
        return false;
    }

    static bool ParseIntToken(const std::string& input, int& out)
    {
        const std::string token = TrimCopy(input);
        if (token.empty())
            return false;

        char* endPtr = nullptr;
        const long value = std::strtol(token.c_str(), &endPtr, 10);
        if (!endPtr || *endPtr != '\0')
            return false;

        out = static_cast<int>(value);
        return true;
    }

    static bool ParseFloatToken(const std::string& input, float& out)
    {
        std::string token = TrimCopy(input);
        if (token.empty())
            return false;

        std::replace(token.begin(), token.end(), ',', '.');

        std::istringstream iss(token);
        iss.imbue(std::locale::classic());

        float value = 0.f;
        char extra = 0;
        if (!(iss >> value))
            return false;
        if (iss >> extra)
            return false;

        out = value;
        return true;
    }

    static std::string FloatToString(float value)
    {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss << std::fixed << std::setprecision(6) << value;
        std::string out = oss.str();

        size_t trimPos = out.find_last_not_of('0');
        if (trimPos != std::string::npos)
        {
            out.erase(trimPos + 1);
            if (!out.empty() && out.back() == '.')
                out.push_back('0');
        }
        return out;
    }

    static bool ParseColorToken(const std::string& input, ImVec4& out)
    {
        std::string token = input;
        std::replace(token.begin(), token.end(), ';', ',');
        std::stringstream ss(token);
        std::string part;
        float values[4] = { 0.f, 0.f, 0.f, 1.f };

        for (int i = 0; i < 4; ++i)
        {
            if (!std::getline(ss, part, ','))
                return false;
            if (!ParseFloatToken(part, values[i]))
                return false;
        }

        out = ImVec4(values[0], values[1], values[2], values[3]);
        return true;
    }

    static std::string ColorToString(const ImVec4& color)
    {
        return FloatToString(color.x) + "," + FloatToString(color.y) + "," +
            FloatToString(color.z) + "," + FloatToString(color.w);
    }
}

void Feature::BindConfigInternal(const char* key, ConfigFieldType type, void* ptr, size_t size)
{
    if (!key || !key[0] || !ptr)
        return;

    const std::string normalizedKey = NormalizeConfigKey(key);
    if (normalizedKey.empty())
        return;

    ConfigFieldBinding binding{};
    binding.type = type;
    binding.ptr = ptr;
    binding.size = size;
    configBindings_[normalizedKey] = binding;

    auto pendingIt = pendingConfigValues_.find(normalizedKey);
    if (pendingIt == pendingConfigValues_.end())
        return;

    const std::string pendingValue = pendingIt->second;
    pendingConfigValues_.erase(pendingIt);
    TryApplyConfigValue(normalizedKey, pendingValue);
}

void Feature::BindConfigBool(const char* key, bool* value)
{
    BindConfigInternal(key, ConfigFieldType::Bool, value, 0);
}

void Feature::BindConfigInt(const char* key, int* value)
{
    BindConfigInternal(key, ConfigFieldType::Int, value, 0);
}

void Feature::BindConfigFloat(const char* key, float* value)
{
    BindConfigInternal(key, ConfigFieldType::Float, value, 0);
}

void Feature::BindConfigString(const char* key, char* value, size_t capacity)
{
    BindConfigInternal(key, ConfigFieldType::String, value, capacity);
}

void Feature::BindConfigColor(const char* key, ImVec4* value)
{
    BindConfigInternal(key, ConfigFieldType::Color4, value, 0);
}

bool Feature::TryApplyConfigValue(const std::string& key, const std::string& value)
{
    const std::string normalizedKey = NormalizeConfigKey(key);
    auto it = configBindings_.find(normalizedKey);
    if (it == configBindings_.end())
        return false;

    ConfigFieldBinding& binding = it->second;
    switch (binding.type)
    {
    case ConfigFieldType::Bool:
    {
        bool parsed = false;
        if (!ParseBoolToken(value, parsed))
            return false;
        *reinterpret_cast<bool*>(binding.ptr) = parsed;
        return true;
    }
    case ConfigFieldType::Int:
    {
        int parsed = 0;
        if (!ParseIntToken(value, parsed))
            return false;
        *reinterpret_cast<int*>(binding.ptr) = parsed;
        return true;
    }
    case ConfigFieldType::Float:
    {
        float parsed = 0.f;
        if (!ParseFloatToken(value, parsed))
            return false;
        *reinterpret_cast<float*>(binding.ptr) = parsed;
        return true;
    }
    case ConfigFieldType::String:
    {
        if (!binding.ptr || binding.size == 0)
            return false;
        strncpy_s(reinterpret_cast<char*>(binding.ptr), binding.size, value.c_str(), _TRUNCATE);
        return true;
    }
    case ConfigFieldType::Color4:
    {
        ImVec4 parsed{};
        if (!ParseColorToken(value, parsed))
            return false;
        *reinterpret_cast<ImVec4*>(binding.ptr) = parsed;
        return true;
    }
    }

    return false;
}

void Feature::ImportConfigValue(const std::string& key, const std::string& value)
{
    const std::string normalizedKey = NormalizeConfigKey(key);
    if (normalizedKey.empty())
        return;

    if (TryApplyConfigValue(normalizedKey, value))
        return;

    if (ImportCustomConfigValue(normalizedKey, value))
        return;

    pendingConfigValues_[normalizedKey] = value;
}

std::vector<std::pair<std::string, std::string>> Feature::ExportConfigValues() const
{
    std::vector<std::pair<std::string, std::string>> out;
    out.reserve(configBindings_.size());

    for (const auto& kv : configBindings_)
    {
        const std::string& key = kv.first;
        const ConfigFieldBinding& binding = kv.second;
        if (!binding.ptr)
            continue;

        std::string value;
        switch (binding.type)
        {
        case ConfigFieldType::Bool:
            value = *reinterpret_cast<const bool*>(binding.ptr) ? "1" : "0";
            break;
        case ConfigFieldType::Int:
            value = std::to_string(*reinterpret_cast<const int*>(binding.ptr));
            break;
        case ConfigFieldType::Float:
            value = FloatToString(*reinterpret_cast<const float*>(binding.ptr));
            break;
        case ConfigFieldType::String:
            value = std::string(reinterpret_cast<const char*>(binding.ptr));
            break;
        case ConfigFieldType::Color4:
            value = ColorToString(*reinterpret_cast<const ImVec4*>(binding.ptr));
            break;
        }

        out.emplace_back(key, value);
    }

    ExportCustomConfigValues(out);

    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        });

    return out;
}

FeatureManager& GetFeatureManager()
{
    static FeatureManager manager;
    return manager;
}
