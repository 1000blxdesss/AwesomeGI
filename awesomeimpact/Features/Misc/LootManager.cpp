#include "LootManager.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

static LootManagerFeature* self{ nullptr };
LootManagerFeature::LootManagerFeature()
    : Feature({ "card_loot_manager", XS("Loot Manager"), XS("Collect items automatically"), "NONE", FeatureTab::Misc })
{
    self = this;
}

void LootManagerFeature::ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const
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
    auto serialize = [&](const std::vector<CapsuleEntry>& caps) {
        std::ostringstream ss;
        ss.imbue(std::locale::classic());
        ss << std::fixed << std::setprecision(6);
        bool first = true;
        for (const auto& e : caps) {
            if (e.label.empty()) continue;
            if (!first) ss << ';';
            first = false;
            ss << hexEncode(e.label) << '@'
                << e.color.x << ',' << e.color.y << ','
                << e.color.z << ',' << e.color.w;
        }
        return ss.str();
        };

    out.emplace_back("loot_whitelist_state", serialize(whitelist_));
    out.emplace_back("loot_blacklist_state", serialize(blacklist_));
}

bool LootManagerFeature::ImportCustomConfigValue(const std::string& key, const std::string& value)
{
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
    auto parseCapsules = [&](const std::string& raw, std::vector<CapsuleEntry>& target) {
        target.clear();
        if (raw.empty()) return;
        size_t start = 0;
        while (start <= raw.size() && target.size() < 1024) {
            const size_t sep = raw.find(';', start);
            const auto item = raw.substr(start, sep == std::string::npos ? std::string::npos : sep - start);
            start = (sep == std::string::npos) ? raw.size() + 1 : sep + 1;
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
            target.push_back({ label, { rgba[0], rgba[1], rgba[2], rgba[3] } });
        }
        };

    if (key == "loot_whitelist_state") { parseCapsules(value, whitelist_); return true; }
    if (key == "loot_blacklist_state") { parseCapsules(value, blacklist_); return true; }
    return false;
}

namespace {

    std::vector<std::tuple<void*, uint32_t, std::chrono::steady_clock::time_point>> g_lootQueue;

    UPDATE_HOOK(Loot)
    {
        if (!self || !self->Active() || g_lootQueue.empty()) return;
        const auto now = std::chrono::steady_clock::now();

        for (auto it = g_lootQueue.begin(); it != g_lootQueue.end(); )
        {
            if (now >= std::get<2>(*it))
            {
                if (self->GetAutoPickUp())
                    UnityUtils::ItemModule_PickItem(std::get<0>(*it), std::get<1>(*it));
                it = g_lootQueue.erase(it);
            }
            else ++it;
        }
    }

    inline float DistanceToEntity(void* entity)
    {
        const auto avatar = OtherUtils::AvatarManager();
        if (!avatar) return 0.f;
        return UnityUtils::GetRelativePosition_Entity(avatar).distance(UnityUtils::GetRelativePosition_Entity(entity));
    }

}

void* (*LCSelectPickup_AddInteeBtnByID_Orig)(void*, void*) = nullptr;
void* LCSelectPickup_AddInteeBtnByID_Hook(void* lcSelectPickup, void* entity)
{
    auto* res = LCSelectPickup_AddInteeBtnByID_Orig(lcSelectPickup, entity);
    if (!self || !self->Active() || !entity) return res;

    auto* itemModuleName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::ItemModule());

    auto* itemModule = UnityUtils::GetSingletonInstance(UnityUtils::get_SingletonManager(), itemModuleName);
    if (!itemModule) return res;

    const auto runtimeID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::runtimeID);

    float lo = ImClamp(self->GetDelayMin(), 0.f, 5000.f);
    float hi = ImClamp(self->GetDelayMax(), 0.f, 5000.f);
    if (hi < lo) std::swap(lo, hi);

    const int delayMs = static_cast<int>(lo) + (std::rand() % static_cast<int>(hi - lo + 1));
    g_lootQueue.push_back({
        itemModule, runtimeID,
        std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs)
        });

    return res;
}

bool (*LCSelectPickup_IsInPosition_Orig)(void*, void*) = nullptr;
bool LCSelectPickup_IsInPosition_Hook(void* __this, void* baseEntity)
{
    const bool result = LCSelectPickup_IsInPosition_Orig(__this, baseEntity);
    if (!self || !self->Active() || !self->IsCustomRange())
        return result;

    const float dist = DistanceToEntity(baseEntity);
    return dist > 0.f ? dist <= self->GetRadius() : result;
}

bool (*LCSelectPickup_IsOutPosition_Orig)(void*, void*) = nullptr;
bool LCSelectPickup_IsOutPosition_Hook(void* __this, void* baseEntity)
{
    const bool result = LCSelectPickup_IsOutPosition_Orig(__this, baseEntity);
    if (!self || !self->Active() || !self->IsCustomRange())
        return result;

    const float dist = DistanceToEntity(baseEntity);
    return dist > 0.f ? dist <= self->GetRadius() : result;
}

float LootManagerFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiRangeSliderRow("loot_delay", XS("Loot Delay (ms)"),
        &delayMin_, &delayMax_, 0.f, 5000.f, "%.0f", ImVec2(x, y), width, true) + 4.f;

    y += GuiToggleTextRow("pickup_enabled", XS("Auto PickUp"),
        XS("Enabled"), autoEnabled_, ImVec2(x, y), width) + 10.f;

    y += GuiToggleTextRow("loot_custom_range_toggle", XS("Custom Dist."),
        XS("Enabled"), customRangeEnabled_, ImVec2(x, y), width) + 8.f;

    if (customRangeEnabled_)
    {
        y += GuiSliderRow("loot_custom_range", XS("Distance"),
            &customRange_, 1.f, 45.f, "%.0f", ImVec2(x, y), width, true) + 10.f;
    }

    bool addWhite = false, addBlack = false;
    y += GuiDualListInputRow("loot_lists", XS("Specifics"), XS("Blacklist"),
        whitelistBuffer_, IM_ARRAYSIZE(whitelistBuffer_),
        blacklistBuffer_, IM_ARRAYSIZE(blacklistBuffer_),
        addWhite, addBlack, ImVec2(x, y), width);

    if (addWhite)
    {
        const auto& accent = GuiActiveAccent();
        GuiAddCapsuleTokens(whitelistBuffer_, whitelist_, { accent.x, accent.y, accent.z, 1.f });
        whitelistBuffer_[0] = '\0';
    }
    if (addBlack)
    {
        GuiAddCapsuleTokens(blacklistBuffer_, blacklist_, { 0.93f, 0.38f, 0.46f, 1.f });
        blacklistBuffer_[0] = '\0';
    }

    y += 6.f;

    const float whH = GuiCapsuleListRowEx("loot_whitelist_capsules", whitelist_, ImVec2(x, y), width, nullptr, true, true);
    if (whH > 0.f) y += whH + 10.f;

    const float blH = GuiCapsuleListRowEx("loot_blacklist_capsules", blacklist_, ImVec2(x, y), width, nullptr, true, true);
    if (blH > 0.f) y += blH + 10.f;

    y += GuiToggleTextRow("pickup_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 8.f;

    return (y - detailStart.y) + 12.f;
}

void LootManagerFeature::OnShutdown()
{
    SetEnabled(false);
}
