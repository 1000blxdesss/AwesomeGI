#include "DefaultProfile.h"

#include "../../GUI/gui.h"
#include "../../Logger/Logger.h"
#include "../../Utils/test02.h"

#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace
{
    static std::string TrimCopy(const std::string& text)
    {
        size_t begin = 0;
        while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])))
            ++begin;

        size_t end = text.size();
        while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])))
            --end;

        return text.substr(begin, end - begin);
    }

    static std::string RemoveQuotes(std::string text)
    {
        text = TrimCopy(text);
        while (!text.empty() && (text.front() == '"' || text.front() == '\''))
            text.erase(text.begin());
        while (!text.empty() && (text.back() == '"' || text.back() == '\''))
            text.pop_back();
        return TrimCopy(text);
    }

    static std::string SanitizeFileName(std::string name)
    {
        for (char& ch : name)
        {
            const bool invalid = ch < 32 || ch == '<' || ch == '>' || ch == ':' || ch == '"' ||
                ch == '/' || ch == '\\' || ch == '|' || ch == '?' || ch == '*';
            if (invalid)
                ch = '_';
        }

        name = TrimCopy(name);
        while (!name.empty() && (name.back() == '.' || name.back() == ' '))
            name.pop_back();

        return name;
    }

    static std::string EnsureCfgDisplayName(std::string requested)
    {
        requested = RemoveQuotes(requested);
        requested = SanitizeFileName(requested);

        if (requested.empty())
            requested = "default";

        const std::string lower = [&]()
            {
                std::string copy = requested;
                std::transform(copy.begin(), copy.end(), copy.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                return copy;
            }();

        if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".cfg")
            requested.erase(requested.size() - 4);

        if (requested.rfind("2802_", 0) == 0)
            requested.erase(0, 5);
        else if (requested.rfind("2802", 0) == 0)
            requested.erase(0, 4);

        requested = TrimCopy(requested);
        if (requested.empty())
            requested = "default";

        return requested;
    }

    static std::string EnsureCfgFileBaseName(std::string requested)
    {
        requested = EnsureCfgDisplayName(std::move(requested));
        return "2802_" + requested;
    }

    static std::string DisplayNameFromFileBase(std::string base)
    {
        if (base.rfind("2802_", 0) == 0)
            base.erase(0, 5);
        else if (base.rfind("2802", 0) == 0)
            base.erase(0, 4);

        base = TrimCopy(base);
        if (base.empty())
            base = "default";

        return base;
    }

    static std::string GetSelfModuleDirectory()
    {
        HMODULE module = nullptr;
        if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetSelfModuleDirectory), &module) || !module)
        {
            return ".";
        }

        char path[MAX_PATH] = {};
        const DWORD copied = GetModuleFileNameA(module, path, MAX_PATH);
        if (copied == 0 || copied >= MAX_PATH)
            return ".";

        std::string full(path);
        const size_t slash = full.find_last_of("\\/");
        if (slash == std::string::npos)
            return ".";
        return full.substr(0, slash);
    }

    static std::string GetConfigRootDirectory()
    {
        char overrideDir[MAX_PATH] = {};
        const DWORD overrideLen = GetEnvironmentVariableA("AWESOMEIMPACT_CFG_DIR", overrideDir, MAX_PATH);
        if (overrideLen > 0 && overrideLen < MAX_PATH)
        {
            std::string trimmed = RemoveQuotes(overrideDir);
            if (!trimmed.empty())
            {
                CreateDirectoryA(trimmed.c_str(), nullptr);
                return trimmed;
            }
        }

        char localAppData[MAX_PATH] = {};
        const DWORD copied = GetEnvironmentVariableA("LOCALAPPDATA", localAppData, MAX_PATH);
        if (copied > 0 && copied < MAX_PATH)
        {
            std::string base(localAppData);
            if (!base.empty() && base.back() != '\\' && base.back() != '/')
                base.push_back('\\');
            base += "AwesomeImpact";
            CreateDirectoryA(base.c_str(), nullptr);
            return base;
        }

        return GetSelfModuleDirectory();
    }

    static std::string EnsureProfilesDirectory()
    {
        std::string path = GetConfigRootDirectory() + "\\profiles";
        CreateDirectoryA(path.c_str(), nullptr);
        return path;
    }

    static std::string BuildProfilePath(const std::string& anyName)
    {
        const std::string base = EnsureCfgFileBaseName(anyName);
        return EnsureProfilesDirectory() + "\\" + base + ".cfg";
    }

    static std::string FileNameWithoutExtension(const std::string& fileName)
    {
        const size_t dot = fileName.find_last_of('.');
        if (dot == std::string::npos)
            return fileName;
        return fileName.substr(0, dot);
    }

    static std::string EncodeCfgValue(const std::string& value)
    {
        std::string out;
        out.reserve(value.size() + 8);
        for (char c : value)
        {
            if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else if (c == '\r') out += "\\r";
            else out.push_back(c);
        }
        return out;
    }

    static std::string DecodeCfgValue(const std::string& value)
    {
        std::string out;
        out.reserve(value.size());

        for (size_t i = 0; i < value.size(); ++i)
        {
            char c = value[i];
            if (c == '\\' && (i + 1) < value.size())
            {
                char n = value[i + 1];
                if (n == 'n') { out.push_back('\n'); ++i; continue; }
                if (n == 'r') { out.push_back('\r'); ++i; continue; }
                if (n == '\\') { out.push_back('\\'); ++i; continue; }
            }
            out.push_back(c);
        }

        return out;
    }

    static bool ParseBoolValue(const std::string& input, bool& out)
    {
        std::string token = TrimCopy(input);
        std::transform(token.begin(), token.end(), token.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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

    static bool ParseIntValue(const std::string& input, int& out)
    {
        std::string token = TrimCopy(input);
        if (token.empty())
            return false;

        char* endPtr = nullptr;
        long parsed = std::strtol(token.c_str(), &endPtr, 10);
        if (!endPtr || *endPtr != '\0')
            return false;

        out = static_cast<int>(parsed);
        return true;
    }
}

DefaultProfileFeature::DefaultProfileFeature()
    : Feature({ "card_save_setting", XS("Save Setting"), XS("Save and load .cfg profiles"), "NONE", FeatureTab::Profiles })
{
    strncpy_s(profileNameInput_, sizeof(profileNameInput_), "default", _TRUNCATE);
    RefreshProfiles();
}

void DefaultProfileFeature::SetProfileNameBuffer(const std::string& value)
{
    strncpy_s(profileNameInput_, sizeof(profileNameInput_), value.c_str(), _TRUNCATE);
}

void DefaultProfileFeature::ShowStatus(const std::string& text, float seconds)
{
    SetStatus(text, seconds);
}

void DefaultProfileFeature::TickStatus(float deltaSeconds)
{
    if (statusTimer_ > 0.f)
    {
        statusTimer_ -= deltaSeconds;
        if (statusTimer_ <= 0.f)
            status_.clear();
    }
}

void DefaultProfileFeature::SetStatus(const std::string& text, float seconds)
{
    status_ = text;
    statusTimer_ = seconds;
}

void DefaultProfileFeature::RefreshProfiles()
{
    profileNames_.clear();

    const std::string dir = EnsureProfilesDirectory();
    const std::string mask = dir + "\\2802*.cfg";

    WIN32_FIND_DATAA fd{};
    HANDLE hFind = FindFirstFileA(mask.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        const std::string fileName = fd.cFileName;
        const std::string base = FileNameWithoutExtension(fileName);
        profileNames_.push_back(DisplayNameFromFileBase(base));
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

    std::sort(profileNames_.begin(), profileNames_.end());
    profileNames_.erase(std::unique(profileNames_.begin(), profileNames_.end()), profileNames_.end());
}

bool DefaultProfileFeature::SaveProfile(const std::string& requestedName, std::string& outStatus)
{
    const std::string displayName = EnsureCfgDisplayName(requestedName);
    const std::string path = BuildProfilePath(displayName);

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open())
    {
        outStatus = XS("Can't save file");
        return false;
    }

    out << "b=2802\n";

    auto& all = GetFeatureManager().All();
    for (const auto& featurePtr : all)
    {
        Feature* feature = featurePtr.get();
        if (!feature || !feature->Info().id)
            continue;

        out << "\n[feature." << feature->Info().id << "]\n";
        out << "enabled=" << (feature->Enabled() ? 1 : 0) << "\n";
        out << "key=" << feature->KeybindKey() << "\n";
        out << "hold=" << (feature->KeybindHold() ? 1 : 0) << "\n";

        const auto values = feature->ExportConfigValues();
        for (const auto& kv : values)
        {
            out << kv.first << "=" << EncodeCfgValue(kv.second) << "\n";
        }
    }

    out.flush();
    if (!out.good())
    {
        outStatus = XS("Write failed");
        return false;
    }

    outStatus = std::string("Saved ") + displayName + ".cfg";
    return true;
}

bool DefaultProfileFeature::LoadProfile(const std::string& profileName, std::string& outStatus)
{
    const std::string path = BuildProfilePath(profileName);
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
        outStatus = XS("Can't open file");
        return false;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
    std::string currentSection;
    std::string line;
    bool magicChecked = false;

    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (!line.empty() && static_cast<unsigned char>(line[0]) == 0xEF)
        {
            if (line.size() >= 3 &&
                static_cast<unsigned char>(line[1]) == 0xBB &&
                static_cast<unsigned char>(line[2]) == 0xBF)
            {
                line.erase(0, 3);
            }
        }

        std::string trimmed = TrimCopy(line);
        if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#')
            continue;

        if (!magicChecked)
        {
            magicChecked = true;
            if (trimmed != "b=2802")
            {
                outStatus = XS("Bad cfg header");
                return false;
            }
            continue;
        }

        if (trimmed.front() == '[' && trimmed.back() == ']')
        {
            currentSection = trimmed.substr(1, trimmed.size() - 2);
            continue;
        }

        const size_t eq = line.find('=');
        if (eq == std::string::npos || currentSection.empty())
            continue;

        std::string key = TrimCopy(line.substr(0, eq));
        std::string value = DecodeCfgValue(line.substr(eq + 1));
        if (key.empty())
            continue;

        sections[currentSection][key] = value;
    }

    if (!magicChecked)
    {
        outStatus = XS("Empty cfg");
        return false;
    }

    auto& all = GetFeatureManager().All();
    for (const auto& featurePtr : all)
    {
        Feature* feature = featurePtr.get();
        if (!feature || !feature->Info().id)
            continue;

        const std::string sectionName = std::string("feature.") + feature->Info().id;
        auto secIt = sections.find(sectionName);
        if (secIt == sections.end())
            continue;

        const auto& kvs = secIt->second;

        auto findValue = [&](const char* key) -> const std::string*
            {
                auto it = kvs.find(key);
                if (it == kvs.end())
                    return nullptr;
                return &it->second;
            };

        if (const std::string* holdValue = findValue("hold"))
        {
            bool parsed = feature->KeybindHold();
            if (ParseBoolValue(*holdValue, parsed))
                feature->SetKeybindHold(parsed);
        }

        if (const std::string* keyValue = findValue("key"))
        {
            int parsed = feature->KeybindKey();
            if (ParseIntValue(*keyValue, parsed))
                feature->SetKeybindKey(parsed, true);
        }


        for (const auto& pair : kvs)
        {
            if (pair.first == "enabled" || pair.first == "key" || pair.first == "hold")
                continue;
            feature->ImportConfigValue(pair.first, pair.second);
        }

        if (const std::string* enabledValue = findValue("enabled"))
        {
            bool parsed = feature->Enabled();
            if (ParseBoolValue(*enabledValue, parsed))
                feature->SetEnabled(parsed);
        }

        for (const auto& pair : kvs)
        {
            if (pair.first == "enabled" || pair.first == "key" || pair.first == "hold")
                continue;
            feature->ImportConfigValue(pair.first, pair.second);
        }
    }

    outStatus = std::string("Loaded ") + EnsureCfgDisplayName(profileName) + ".cfg";
    return true;
}

bool DefaultProfileFeature::DeleteProfile(const std::string& profileName, std::string& outStatus)
{
    const std::string path = BuildProfilePath(profileName);
    if (!DeleteFileA(path.c_str()))
    {
        outStatus = XS("Delete failed");
        return false;
    }

    outStatus = std::string("Deleted ") + EnsureCfgDisplayName(profileName) + ".cfg";
    return true;
}

float DefaultProfileFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    bool saveTriggered = false;
    y += GuiInputConfirmRow("profiles_save_input", XS("Save Setting"), profileNameInput_,
        IM_ARRAYSIZE(profileNameInput_), saveTriggered, ImVec2(x, y), width);
    y += 8.f;

    if (saveTriggered)
    {
        std::string status;
        if (SaveProfile(profileNameInput_, status))
            RefreshProfiles();
        SetStatus(status, 3.0f);
    }

    if (!status_.empty())
    {
        y += GuiDetailRowText(XS("Status"), status_.c_str(), ImVec2(x, y), width, false);
        y += 4.f;
    }

    return (y - detailStart.y) + 12.f;
}
