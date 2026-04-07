#include "LootMagnet.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

static LootMagnetFeature* self{ nullptr };
LootMagnetFeature::LootMagnetFeature()
    : Feature({ "card_loot_magnet", XS("Object Magnet"), XS("Pull object toward the player"), "NONE", FeatureTab::Misc })
{
    self = this;
}

void LootMagnetFeature::ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const
{
    auto hexEncode = [](const std::string& text) {
        static const char* kHex = "0123456789ABCDEF";
        std::string r;
        r.reserve(text.size() * 2);
        for (unsigned char c : text) {
            r.push_back(kHex[(c >> 4) & 0x0F]);
            r.push_back(kHex[c & 0x0F]);
        }
        return r;
        };

    std::ostringstream ss;
    ss.imbue(std::locale::classic());
    ss << std::fixed << std::setprecision(6);
    bool first = true;
    for (const auto& e : names_) {
        if (e.label.empty()) continue;
        if (!first) ss << ';';
        first = false;
        ss << hexEncode(e.label) << '@'
            << e.color.x << ',' << e.color.y << ','
            << e.color.z << ',' << e.color.w;
    }
    out.emplace_back("loot_magnet_capsules_state", ss.str());
}

bool LootMagnetFeature::ImportCustomConfigValue(const std::string& key, const std::string& value)
{
    if (key != "loot_magnet_capsules_state") return false;

    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
        return -1;
        };
    auto hexDecode = [&](const std::string& hex, std::string& out) -> bool {
        if (hex.size() % 2 != 0) return false;
        out.clear();
        out.reserve(hex.size() / 2);
        for (size_t i = 0; i < hex.size(); i += 2) {
            int hi = nibble(hex[i]), lo = nibble(hex[i + 1]);
            if (hi < 0 || lo < 0) return false;
            out.push_back(static_cast<char>((hi << 4) | lo));
        }
        return true;
        };
    auto parseFloat = [](std::string tok, float& out) -> bool {
        if (tok.empty()) return false;
        std::replace(tok.begin(), tok.end(), ',', '.');
        std::istringstream ss(tok);
        ss.imbue(std::locale::classic());
        return (ss >> out) && ss.eof();
        };

    names_.clear();
    if (value.empty()) return true;

    size_t start = 0;
    while (start <= value.size() && names_.size() < 1024) {
        const size_t sep = value.find(';', start);
        const auto item = value.substr(start, sep == std::string::npos ? std::string::npos : sep - start);
        start = (sep == std::string::npos) ? value.size() + 1 : sep + 1;
        if (item.empty()) continue;

        const size_t at = item.find('@');
        if (at == std::string::npos) continue;

        std::string label;
        if (!hexDecode(item.substr(0, at), label) || label.empty()) continue;

        float rgba[4] = { 1, 1, 1, 1 };
        std::stringstream css(item.substr(at + 1));
        css.imbue(std::locale::classic());
        bool ok = true;
        for (int i = 0; i < 4 && ok; ++i) {
            std::string part;
            ok = std::getline(css, part, ',') && parseFloat(part, rgba[i]);
        }
        if (!ok) continue;

        names_.push_back({ label, { rgba[0], rgba[1], rgba[2], rgba[3] } });
    }
    return true;
}

namespace {

    inline std::string GetEntityName(void* entity)
    {
        auto* go = UnityUtils::BaseEntity_rootObject(entity);
        if (!go) return {};
        auto* str = UnityUtils::Object_get_name(go);
        return (str && str->length > 0) ? OtherUtils::Utf16ToUtf8(str->chars, str->length) : std::string();
    }

    inline bool MatchesAnyName(const std::string& name, const std::vector<CapsuleEntry>& names)
    {
        if (name.empty()) return false;
        return std::any_of(names.begin(), names.end(), [&](const CapsuleEntry& e) {
            return !e.label.empty() && name.find(e.label) != std::string::npos;
            });
    }

    UPDATE_HOOK(Magnet)
    {
        if (!self || !self->Active()) return;

        const auto localPlayer = OtherUtils::AvatarManager();
        if (!localPlayer) return;

        const auto& names = self->GetNames();
        if (names.empty()) return;

        const auto playerPos = UnityUtils::GetRelativePosition_Entity(localPlayer);
        OtherUtils::ForEachEntity([&](void* entity)
            {
                if (MatchesAnyName(GetEntityName(entity), names))
                    UnityUtils::SetRelativePosition(entity, playerPos, true);
            });
    }

}

float LootMagnetFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    bool addReq = false;
    y += GuiInputAddRow("loot_magnet_names", XS("Names"),
        buffer_, IM_ARRAYSIZE(buffer_), addReq, ImVec2(x, y), width);

    if (addReq)
    {
        const auto& accent = GuiActiveAccent();
        GuiAddCapsuleTokens(buffer_, names_, { accent.x, accent.y, accent.z, 1.f });
        buffer_[0] = '\0';
    }

    y += 6.f;

    const float chipH = GuiCapsuleListRow("loot_magnet_capsules", names_, ImVec2(x, y), width);
    if (chipH > 0.f) y += chipH + 10.f;

    y += GuiToggleTextRow("magnet_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 10.f;

    return (y - detailStart.y) + 12.f;
}

void LootMagnetFeature::OnShutdown()
{
    SetEnabled(false);
}
