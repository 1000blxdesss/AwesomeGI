#include "Esp.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

static EspFeature* self{ nullptr };
EspFeature::EspFeature()
    : Feature({ "card_esp", XS("ESP"), XS("Highlights objects"), "NONE", FeatureTab::Visual })
{
    self = this;
}


void EspFeature::ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const
{
    auto hexEncode = [](const std::string& text) -> std::string {
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
    for (const auto& e : includeList_) {
        if (e.label.empty()) continue;
        if (!first) ss << ';';
        first = false;
        ss << hexEncode(e.label) << '@'
            << e.color.x << ',' << e.color.y << ','
            << e.color.z << ',' << e.color.w;
    }
    out.emplace_back("esp_capsules_state", ss.str());
}

bool EspFeature::ImportCustomConfigValue(const std::string& key, const std::string& value)
{
    if (key != "esp_capsules_state") return false;

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

    includeList_.clear();
    if (value.empty()) return true;

    size_t start = 0;
    while (start <= value.size() && includeList_.size() < 1024) {
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

        includeList_.push_back({ label, { rgba[0], rgba[1], rgba[2], rgba[3] } });
    }
    return true;
}


float EspFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Box"), XS("Corner") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;
    auto* dl = ImGui::GetWindowDrawList();

    y += GuiDropdownRow("esp_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width) + 10.f;

    bool addReq = false;
    y += GuiInputAddRow("esp_include", XS("Include"), inputBuffer_,
        IM_ARRAYSIZE(inputBuffer_), addReq, ImVec2(x, y), width);

    if (addReq)
    {
        auto trimEnds = [](std::string& s) {
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))  s.pop_back();
            };
        auto existsCI = [&](const std::string& tok) {
            return std::any_of(includeList_.begin(), includeList_.end(), [&](const CapsuleEntry& e) {
                if (e.label.size() != tok.size()) return false;
                for (size_t j = 0; j < tok.size(); ++j)
                    if (std::tolower(static_cast<unsigned char>(e.label[j])) !=
                        std::tolower(static_cast<unsigned char>(tok[j]))) return false;
                return true;
                });
            };

        std::string raw = inputBuffer_;
        bool added = false;
        size_t pos = 0;
        while (pos < raw.size()) {
            const size_t comma = raw.find(',', pos);
            auto tok = raw.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos);
            trimEnds(tok);
            if (!tok.empty() && !existsCI(tok)) {
                includeList_.push_back({ tok, { 1, 1, 1, 1 } });
                added = true;
            }
            if (comma == std::string::npos) break;
            pos = comma + 1;
        }
        if (added) inputBuffer_[0] = '\0';
    }

    y += 6.f;
    dl->AddLine({ x, y }, { x + width, y }, IM_COL32(90, 90, 104, 140), 1.1f);
    y += 10.f;

    const float chipH = GuiCapsuleListRow("esp_capsules", includeList_, ImVec2(x, y), width);
    if (chipH > 0.f) y += chipH + 12.f;

    y += GuiToggleTextRow("esp_filled", XS("Filled"), XS("Enabled"), filled_, ImVec2(x, y), width) + 6.f;
    y += GuiToggleTextRow("esp_tracers", XS("Tracers"), XS("Enabled"), tracers_, ImVec2(x, y), width) + 6.f;
    y += GuiToggleTextRow("esp_spirits", XS("Spirits"), XS("Enabled"), spirits_, ImVec2(x, y), width) + 6.f;
    y += GuiToggleTextRow("esp_highlight", XS("Highlight"), XS("Enabled"), highlight_, ImVec2(x, y), width) + 6.f;
    y += GuiToggleTextRow("esp_hold_key", XS("Key"), XS("Hold"), holdKey_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}


namespace {

    std::vector<CapsuleEntry> g_includeSnap;
    ImVec4 g_activeAccent = { 0.72f, 0.18f, 0.56f, 1.f };

    inline bool IsDefaultAccent(const ImVec4& c)
    {
        const auto& a = GuiActiveAccent();
        constexpr float eps = 0.02f;
        return std::abs(c.x - a.x) < eps && std::abs(c.y - a.y) < eps && std::abs(c.z - a.z) < eps;
    }

    inline ImU32 ResolveEntityColor(const std::string& name, const std::vector<CapsuleEntry>& list, bool& found)
    {
        found = false;
        if (list.empty()) { found = true; return IM_COL32(255, 255, 255, 255); }
        for (const auto& e : list) {
            if (name.find(e.label) != std::string::npos) {
                found = true;
                return IsDefaultAccent(e.color) ? IM_COL32(255, 255, 255, 255) : ImGui::ColorConvertFloat4ToU32(e.color);
            }
        }
        return IM_COL32(255, 255, 255, 255);
    }

    inline ImVec4 ResolveEntityColor4(const std::string& name, const std::vector<CapsuleEntry>& list, bool& found)
    {
        found = false;
        if (list.empty()) { found = true; return { 1, 1, 1, 1 }; }
        for (const auto& e : list) {
            if (name.find(e.label) != std::string::npos) {
                found = true;
                return IsDefaultAccent(e.color) ? ImVec4(1, 1, 1, 1) : e.color;
            }
        }
        return { 1, 1, 1, 1 };
    }

    inline std::string GetEntityName(void* entity)
    {
        auto* rootObj = UnityUtils::BaseEntity_rootObject(entity);
        if (!rootObj) return {};
        auto* nameStr = UnityUtils::Object_get_name(rootObj);
        return nameStr ? OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length) : std::string();
    }

    struct EntityBounds {
        Vector3 center, extents, min, max;
    };

    inline bool ComputeEntityBounds(void* entity, EntityBounds& out)
    {
        auto* tool = *reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::monoVisualEntityTool);
        if (!tool) return false;

        auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(tool);
        if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000) return false;

        Vector3 mn = { FLT_MAX, FLT_MAX, FLT_MAX };
        Vector3 mx = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
        bool any = false;

        for (uint32_t j = 0; j < renderers->max_length; ++j) {
            auto* r = renderers->vector[j];
            if (!r) continue;
            const auto b = UnityUtils::Renderer_get_bounds(r);
            const auto& c = b.m_Center;
            const auto& e = b.m_Extents;
            if (!std::isfinite(c.x) || !std::isfinite(c.y) || !std::isfinite(c.z)) continue;
            if (!std::isfinite(e.x) || !std::isfinite(e.y) || !std::isfinite(e.z)) continue;
            if (e.x <= 0.01f && e.y <= 0.01f && e.z <= 0.01f) continue;

            mn.x = (std::min)(mn.x, c.x - e.x); mn.y = (std::min)(mn.y, c.y - e.y); mn.z = (std::min)(mn.z, c.z - e.z);
            mx.x = (std::max)(mx.x, c.x + e.x); mx.y = (std::max)(mx.y, c.y + e.y); mx.z = (std::max)(mx.z, c.z + e.z);
            any = true;
        }
        if (!any) return false;

        out.min = mn;
        out.max = mx;
        out.center = { (mn.x + mx.x) * .5f, (mn.y + mx.y) * .5f, (mn.z + mx.z) * .5f };
        out.extents = { (mx.x - mn.x) * .5f, (mx.y - mn.y) * .5f, (mx.z - mn.z) * .5f };
        return true;
    }

    inline void DrawAnimatedTracer(ImDrawList* dl, const ImVec2& from, const ImVec2& to, ImU32 targetColor)
    {
        const ImVec2 delta = { to.x - from.x, to.y - from.y };
        const float len = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (len <= 2.f) return;

        const ImVec2 dir = { delta.x / len, delta.y / len };

        constexpr float kDash = 14.f, kGap = 9.f, kSpeed = 65.f;
        const float t = static_cast<float>(ImGui::GetTime());
        const float cycle = kDash + kGap;
        const float offset = std::fmod(t * kSpeed, cycle);

        const ImColor target(targetColor);
        const ImColor accent(g_activeAccent);

        constexpr float kThickNear = 3.2f, kThickFar = 1.6f;

        for (float d = -offset; d < len; d += cycle) {
            const float segStart = ImClamp(d, 0.f, len);
            const float segEnd = ImClamp(d + kDash, 0.f, len);
            if (segEnd <= segStart) continue;

            const float along = segStart / len;
            const float alpha = ImClamp(target.Value.w * (1.f - along) * 0.8f, 0.1f, target.Value.w * 0.85f);
            ImVec4 c = ImLerp(accent.Value, target.Value, along);
            c.w = alpha;

            const float thick = ImLerp(kThickNear, kThickFar, along);
            const ImVec2 p0 = { from.x + dir.x * segStart, from.y + dir.y * segStart };
            const ImVec2 p1 = { from.x + dir.x * segEnd,   from.y + dir.y * segEnd };

            dl->AddLine(p0, p1, ImGui::GetColorU32({ c.x, c.y, c.z, c.w * 0.25f }), thick * 2.2f);
            dl->AddLine(p0, p1, ImGui::GetColorU32(c), thick);
        }

        const float pulse = 0.5f + 0.5f * std::sin(t * 4.f);
        ImVec4 ring = target.Value; ring.w = 0.18f;
        dl->AddCircle(to, 6.f + pulse * 6.f, ImGui::GetColorU32(ring), 32, 2.f);
    }

    inline ImVec2 ClampToScreenEdge(const ImVec2& from, const ImVec2& to, float w, float h)
    {
        const ImVec2 dir = { to.x - from.x, to.y - from.y };
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.0001f) return { ImClamp(to.x, 0.f, w), ImClamp(to.y, 0.f, h) };

        const ImVec2 d = { dir.x / len, dir.y / len };
        float tMin = 1e9f;

        if (std::abs(d.x) > 0.0001f) {
            const float t = (d.x > 0 ? w - from.x : -from.x) / d.x;
            if (t > 0) tMin = (std::min)(tMin, t);
        }
        if (std::abs(d.y) > 0.0001f) {
            const float t = (d.y > 0 ? h - from.y : -from.y) / d.y;
            if (t > 0) tMin = (std::min)(tMin, t);
        }

        if (tMin >= 1e9f) return { ImClamp(to.x, 0.f, w), ImClamp(to.y, 0.f, h) };
        return { from.x + d.x * tMin, from.y + d.y * tMin };
    }

    struct SpiritSphere {
        void* go = nullptr;
        void* transform = nullptr;
        void* tailLine = nullptr;
        std::array<Vector3, 30> trail{};
        int trailCount = 0, trailHead = 0;
        float stillTime = 0.f, sampleTime = 0.f;
        uintptr_t ownerKey = 0;
    };

    struct SpiritTarget {
        void* owner = nullptr;
        Vector3 center, extents;
        ImU32 color;
    };

    std::vector<SpiritSphere> g_spiritPool;
    std::vector<SpiritTarget> g_spiritTargets;
    int g_spiritUsed = 0;

    inline void ResetSpiritTrail(SpiritSphere& s)
    {
        s.trailCount = s.trailHead = 0;
        s.stillTime = s.sampleTime = 0.f;
        if (s.tailLine) UnityUtils::LineRenderer_set_positionCount(s.tailLine, 0);
    }

    inline void EnsureSpiritTail(SpiritSphere& s, float width)
    {
        if (!s.go) return;
        if (!s.tailLine) {
            auto* lrName = UnityUtils::PtrToStringAnsi((void*)XS("LineRenderer"));
            s.tailLine = UnityUtils::AddComponentInternal(s.go, lrName);
            if (!s.tailLine) return;
            UnityUtils::LineRenderer_set_useWorldSpace(s.tailLine, true);

            auto* mat = UnityUtils::Renderer_get_material(s.tailLine);
            if (mat) {
                static void* shader = UnityUtils::Shader_Find(UnityUtils::PtrToStringAnsi((void*)XS("Sprites/Default")));
                if (shader) UnityUtils::Material_set_shader(mat, shader);
            }
        }
        UnityUtils::LineRenderer_set_startWidth(s.tailLine, width);
        UnityUtils::LineRenderer_set_endWidth(s.tailLine, 0.f);
        UnityUtils::LineRenderer_set_startColor(s.tailLine, { 1, 1, 1, 1 });
        UnityUtils::LineRenderer_set_endColor(s.tailLine, { 1, 1, 1, 0 });
    }

    inline void UpdateSpiritTail(SpiritSphere& s, const Vector3& headPos, float width, float maxJump)
    {
        if (!s.go) return;
        EnsureSpiritTail(s, width);
        if (!s.tailLine) return;

        constexpr float kMinDist = 0.01f, kSamplePeriod = 0.03f, kClearAfter = 0.2f;
        const int cap = static_cast<int>(s.trail.size());
        float dt = ImGui::GetIO().DeltaTime;
        if (dt <= 0.f) dt = 0.016f;

        if (s.trailCount == 0) {
            s.trail[0] = headPos;
            s.trailCount = 1;
            s.trailHead = 1 % cap;
        }
        else {
            const int last = (s.trailHead - 1 + cap) % cap;
            const float dist = s.trail[last].distance(headPos);

            if (dist > maxJump) {
                ResetSpiritTrail(s);
                s.trail[0] = headPos;
                s.trailCount = 1;
                s.trailHead = 1 % cap;
                return;
            }

            if (dist < kMinDist) {
                s.sampleTime += dt;
                if (s.sampleTime >= kSamplePeriod) {
                    s.sampleTime = 0.f;
                    s.trail[s.trailHead] = headPos;
                    s.trailHead = (s.trailHead + 1) % cap;
                    if (s.trailCount < cap) ++s.trailCount;
                    s.stillTime = 0.f;
                }
                else {
                    s.stillTime += dt;
                    s.trail[last] = headPos;
                }
                if (s.stillTime >= kClearAfter) {
                    s.trail[0] = headPos;
                    s.trailCount = 1;
                    s.trailHead = 1 % cap;
                    s.sampleTime = 0.f;
                    UnityUtils::LineRenderer_set_positionCount(s.tailLine, 0);
                    return;
                }
            }
            else {
                s.stillTime = s.sampleTime = 0.f;
                s.trail[s.trailHead] = headPos;
                s.trailHead = (s.trailHead + 1) % cap;
                if (s.trailCount < cap) ++s.trailCount;
            }
        }

        if (s.trailCount < 2) {
            UnityUtils::LineRenderer_set_positionCount(s.tailLine, 0);
            return;
        }
        UnityUtils::LineRenderer_set_positionCount(s.tailLine, s.trailCount);
        for (int i = 0; i < s.trailCount; ++i)
            UnityUtils::LineRenderer_SetPosition(s.tailLine, i, s.trail[(s.trailHead - 1 - i + cap) % cap]);
    }

    inline void SetSphereColor(void* go, const Color& color)
    {
        if (!go) return;
        auto* rName = UnityUtils::PtrToStringAnsi((void*)XS("Renderer"));
        auto* r = UnityUtils::GameObject_GetComponentByName(go, rName);
        if (!r) return;
        auto* mat = UnityUtils::Renderer_get_sharedMaterial(r);
        if (!mat) return;
        UnityUtils::Material_set_color(mat, color);
        UnityUtils::Material_set_renderQueue(mat, 5000);
    }

    inline SpiritSphere* EnsureSphere(int index, const Color& color, float scale, uintptr_t ownerKey)
    {
        if (index < 0 || index >= 512) return nullptr;
        if (index >= static_cast<int>(g_spiritPool.size()))
            g_spiritPool.resize(index + 1);

        auto& s = g_spiritPool[index];
        if (!s.go) {
            s.go = UnityUtils::GameObject_CreatePrimitive(PrimitiveType::Sphere);
            if (!s.go) return nullptr;

            auto* colName = UnityUtils::PtrToStringAnsi((void*)XS("Collider"));
            auto* col = UnityUtils::GameObject_GetComponentByName(s.go, colName);
            if (col) UnityUtils::Object_Destroy(col);

            auto* trName = UnityUtils::PtrToStringAnsi((void*)XS("Transform"));
            s.transform = UnityUtils::GameObject_GetComponentByName(s.go, trName);
        }
        if (!s.transform) {
            auto* trName = UnityUtils::PtrToStringAnsi((void*)XS("Transform"));
            s.transform = UnityUtils::GameObject_GetComponentByName(s.go, trName);
        }
        if (s.ownerKey != ownerKey) {
            s.ownerKey = ownerKey;
            ResetSpiritTrail(s);
        }
        UnityUtils::GameObject_SetActive(s.go, true);
        SetSphereColor(s.go, color);
        if (s.transform)
            UnityUtils::Transform_set_localScale(s.transform, { scale, scale, scale });
        return &s;
    }

    inline void DeactivateAllSpirits()
    {
        for (auto& s : g_spiritPool) {
            if (s.go) {
                UnityUtils::GameObject_SetActive(s.go, false);
                s.ownerKey = 0;
                ResetSpiritTrail(s);
            }
        }
        g_spiritUsed = 0;
        g_spiritTargets.clear();
    }

    inline void DestroyAllSpirits()
    {
        for (auto& s : g_spiritPool)
            if (s.go) UnityUtils::Object_Destroy(s.go);
        g_spiritPool.clear();
        g_spiritUsed = 0;
        g_spiritTargets.clear();
    }

    void UpdateSpiritsTick()
    {
        if (!self || !self->Active() || !self->IsSpiritsEnabled() || g_spiritTargets.empty()) {
            DeactivateAllSpirits();
            return;
        }

        g_spiritUsed = 0;
        const float t = static_cast<float>(ImGui::GetTime());

        auto cross = [](const Vector3& a, const Vector3& b) -> Vector3 {
            return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
            };
        auto normalize = [](Vector3 v) -> Vector3 {
            const float l = v.magnitude();
            return l > 0.0001f ? v * (1.f / l) : v;
            };

        const Vector3 kAxis = { 0.55f, 0.82f, 0.15f };
        const auto axis = normalize(kAxis);
        Vector3 helper = (std::abs(axis.x) < 0.1f && std::abs(axis.z) < 0.1f)
            ? Vector3{ 1, 0, 0 } : Vector3{ 0, 0, 1 };
        const auto right = normalize(cross(helper, axis));
        const auto up = normalize(cross(axis, right));

        auto buildHead = [&](const Vector3& center, const Vector3& extents, int idx, Vector3& out, float& outR) -> bool {
            const float baseR = ImClamp((std::max)(extents.x, extents.z) * 1.2f, 0.6f, 16.f);
            const float hAmp = ImClamp(extents.y * 0.4f, 0.2f, 6.f);
            if (baseR <= 0.01f || hAmp <= 0.01f) return false;

            const float dir = idx == 0 ? 1.f : -1.f;
            const float phase = IM_PI * 0.6f * idx;
            const float angle = t * (1.55f + 0.25f * idx) * dir + phase;
            const float r = baseR * (0.85f + 0.18f * std::sin(t * 1.1f + phase));
            const float shift = hAmp * 0.35f * std::sin(t * 0.7f + phase);

            out = {
                center.x + right.x * std::cos(angle) * r + up.x * std::sin(angle) * r + axis.x * shift,
                center.y + right.y * std::cos(angle) * r + up.y * std::sin(angle) * r + axis.y * shift,
                center.z + right.z * std::cos(angle) * r + up.z * std::sin(angle) * r + axis.z * shift
            };
            outR = baseR;
            return true;
            };

        for (const auto& tgt : g_spiritTargets) {
            Vector3 headA{}, headB{};
            float baseR = 1.f;
            if (!buildHead(tgt.center, tgt.extents, 0, headA, baseR)) continue;
            float baseRB = baseR;
            buildHead(tgt.center, tgt.extents, 1, headB, baseRB);

            const ImColor c(tgt.color);
            const Color col = { c.Value.x, c.Value.y, c.Value.z, 1.f };
            const float sz = ImClamp(baseR * 0.08f, 0.2f, 1.2f);
            const float tw = ImClamp(sz * 0.7f, 0.06f, 0.6f);
            const float jump = ImClamp(baseR * 3.5f, 2.5f, 30.f);

            const uintptr_t keyBase = reinterpret_cast<uintptr_t>(tgt.owner);

            if (auto* s0 = EnsureSphere(g_spiritUsed++, col, sz, (keyBase << 1) | 0)) {
                if (s0->transform) UnityUtils::Transform_set_position(s0->transform, headA);
                UpdateSpiritTail(*s0, headA, tw, jump);
            }
            if (auto* s1 = EnsureSphere(g_spiritUsed++, col, sz * 0.9f, (keyBase << 1) | 1)) {
                if (s1->transform) UnityUtils::Transform_set_position(s1->transform, headB);
                UpdateSpiritTail(*s1, headB, tw * 0.9f, jump);
            }
        }

        for (int i = g_spiritUsed; i < static_cast<int>(g_spiritPool.size()); ++i)
            if (g_spiritPool[i].go) UnityUtils::GameObject_SetActive(g_spiritPool[i].go, false);
    }

    struct HLMatData { void* material; void* shader; Color color; int zTest, zWrite, cull, renderQueue; };
    struct HLRendData { void* renderer; bool skipGPU; float viewDist; };

    std::vector<HLMatData>     g_hlMats;
    std::vector<HLRendData>    g_hlRends;
    std::unordered_set<void*>  g_hlProcessedMats;
    std::unordered_set<void*>  g_hlProcessedRends;
    bool  g_hlActive = false;
    void* g_hlShader = nullptr;

    void RestoreHighlight()
    {
        if (!g_hlActive) return;

        static int pidZTest = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZTest")));
        static int pidZWrite = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZWrite")));
        static int pidCull = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_Cull")));

        for (auto& d : g_hlMats) {
            if (!d.material) continue;
            if (d.shader) UnityUtils::Material_set_shader(d.material, d.shader);
            UnityUtils::Material_set_color(d.material, d.color);
            UnityUtils::Material_SetInt(d.material, pidZTest, d.zTest);
            UnityUtils::Material_SetInt(d.material, pidZWrite, d.zWrite);
            UnityUtils::Material_SetInt(d.material, pidCull, d.cull);
            UnityUtils::Material_set_renderQueue(d.material, d.renderQueue);
        }
        for (auto& d : g_hlRends) {
            if (!d.renderer) continue;
            UnityUtils::Renderer_set_skipGPUCulling(d.renderer, d.skipGPU);
            UnityUtils::Renderer_set_viewDistanceRatio(d.renderer, d.viewDist);
        }

        g_hlMats.clear(); g_hlRends.clear();
        g_hlProcessedMats.clear(); g_hlProcessedRends.clear();
        g_hlActive = false;
    }

    void ApplyHighlight()
    {
        if (!self || !self->Active() || !self->IsHighlightEnabled()) { RestoreHighlight(); return; }

        if (!g_hlShader) {
            auto* s1 = UnityUtils::PtrToStringAnsi((void*)XS("Hidden/Internal-Colored"));
            auto* s2 = UnityUtils::PtrToStringAnsi((void*)XS("Unlit/Color"));
            g_hlShader = UnityUtils::Shader_Find(s1);
            if (!g_hlShader) g_hlShader = UnityUtils::Shader_Find(s2);
        }
        if (!g_hlShader) return;

        static int pidZTest = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZTest")));
        static int pidZWrite = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZWrite")));
        static int pidCull = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_Cull")));

        const auto& incList = self->GetIncludeList();

        if (!OtherUtils::ForEachEntity([&](void* entity)
            {
                const auto name = GetEntityName(entity);
                bool found = false;
                const auto col4 = ResolveEntityColor4(name, incList, found);
                if (!found) return;

                auto* tool = *reinterpret_cast<void**>(
                    reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::monoVisualEntityTool);
                if (!tool) return;
                auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(tool);
                if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000) return;

                for (uint32_t j = 0; j < renderers->max_length; ++j) {
                    auto* renderer = renderers->vector[j];
                    if (!renderer) continue;

                    if (g_hlProcessedRends.insert(renderer).second) {
                        g_hlRends.push_back({
                            renderer,
                            UnityUtils::Renderer_get_skipGPUCulling(renderer),
                            UnityUtils::Renderer_get_viewDistanceRatio(renderer)
                            });
                    }
                    UnityUtils::Renderer_set_skipGPUCulling(renderer, true);
                    UnityUtils::Renderer_set_viewDistanceRatio(renderer, 1e9f);

                    auto* mats = UnityUtils::Renderer_get_sharedMaterials(renderer);
                    if (!mats || mats->max_length == 0 || mats->max_length > 256) continue;

                    for (uint32_t m = 0; m < mats->max_length; ++m) {
                        auto* mat = mats->vector[m];
                        if (!mat) continue;

                        if (g_hlProcessedMats.insert(mat).second) {
                            g_hlMats.push_back({
                                mat,
                                UnityUtils::Material_get_shader(mat),
                                UnityUtils::Material_get_color(mat),
                                UnityUtils::Material_GetInt(mat, pidZTest),
                                UnityUtils::Material_GetInt(mat, pidZWrite),
                                UnityUtils::Material_GetInt(mat, pidCull),
                                UnityUtils::Material_get_renderQueue(mat)
                                });
                        }

                        UnityUtils::Material_set_shader(mat, g_hlShader);
                        UnityUtils::Material_set_color(mat, { col4.x, col4.y, col4.z, col4.w });
                        UnityUtils::Material_SetInt(mat, pidZTest, 8);
                        UnityUtils::Material_SetInt(mat, pidZWrite, 0);
                        UnityUtils::Material_SetInt(mat, pidCull, 0);
                        UnityUtils::Material_set_renderQueue(mat, 5000);
                    }
                }
            }))
        {
            return;
        }
        g_hlActive = true;
    }


    UPDATE_HOOK(Esp_Spirits) { UpdateSpiritsTick(); }
    UPDATE_HOOK(Esp_Highlight) { ApplyHighlight(); }

}

void EspFeature::OnDraw()
{
    if (!self || !self->Active()) return;

    g_includeSnap = includeList_;
    g_spiritTargets.clear();

    auto* dl = ImGui::GetBackgroundDrawList();
    auto* font = g_fontNameTags ? g_fontNameTags : ImGui::GetFont();
    const float fontSize = font ? font->LegacySize : ImGui::GetFontSize();

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    const auto localAvatar = OtherUtils::AvatarManager();
    if (!localAvatar) return;

    const auto playerPos = UnityUtils::GetRelativePosition_Entity(localAvatar);
    const float screenW = static_cast<float>(UnityUtils::get_width());
    const float screenH = static_cast<float>(UnityUtils::get_height());

    ImVec2 playerFrom = { screenW * 0.5f, screenH * 0.5f };
    if (tracers_) {
        EntityBounds pb{};
        if (ComputeEntityBounds(localAvatar, pb)) {
            const auto ps = OtherUtils::WorldToScreenPointCorrected(camera, pb.center);
            const ImVec2 raw = { ps.x, screenH - ps.y };
            if (ps.z > 0.1f && raw.x >= 0 && raw.x <= screenW && raw.y >= 0 && raw.y <= screenH)
                playerFrom = raw;
        }
    }

    if (!OtherUtils::ForEachEntity([&](void* entity)
        {
            auto name = GetEntityName(entity);
            if (name.empty()) name = XS("Unknown");

            bool found = false;
            const ImU32 color = ResolveEntityColor(name, g_includeSnap, found);
            if (!found) return;

            EntityBounds eb{};
            if (!ComputeEntityBounds(entity, eb)) return;

            const Vector3 corners[8] = {
                { eb.min.x, eb.min.y, eb.min.z }, { eb.max.x, eb.min.y, eb.min.z },
                { eb.max.x, eb.min.y, eb.max.z }, { eb.min.x, eb.min.y, eb.max.z },
                { eb.min.x, eb.max.y, eb.min.z }, { eb.max.x, eb.max.y, eb.min.z },
                { eb.max.x, eb.max.y, eb.max.z }, { eb.min.x, eb.max.y, eb.max.z },
            };

            Vector3 sc[8];
            ImVec2  s2d[8];
            for (int k = 0; k < 8; ++k) {
                sc[k] = OtherUtils::WorldToScreenPointCorrected(camera, corners[k]);
                s2d[k] = { sc[k].x, screenH - sc[k].y };
            }

            if (spirits_)
                g_spiritTargets.push_back({ entity, eb.center, eb.extents, color });

            constexpr float kCornerPct = 0.05f;
            constexpr float kThick = 1.f;

            auto safeLine = [&](int a, int b) {
                if (sc[a].z > 1.f && sc[b].z > 1.f)
                    dl->AddLine(s2d[a], s2d[b], color, kThick);
                };
            auto safeCorner = [&](int from, int to) {
                if (sc[from].z <= 1.f || sc[to].z <= 1.f) return;
                const ImVec2 dir = { (s2d[to].x - s2d[from].x) * kCornerPct,
                                      (s2d[to].y - s2d[from].y) * kCornerPct };
                dl->AddLine(s2d[from], { s2d[from].x + dir.x, s2d[from].y + dir.y }, color, kThick);
                };

            if (modeIndex_ == 1)
            {
                constexpr int edges[][2] = {
                    {0,1},{0,3},{0,4}, {1,0},{1,2},{1,5}, {2,1},{2,3},{2,6}, {3,0},{3,2},{3,7},
                    {4,0},{4,5},{4,7}, {5,1},{5,4},{5,6}, {6,2},{6,5},{6,7}, {7,3},{7,4},{7,6}
                };
                for (const auto& e : edges) safeCorner(e[0], e[1]);
            }
            else
            {
                safeLine(0, 1); safeLine(1, 2); safeLine(2, 3); safeLine(3, 0);
                safeLine(4, 5); safeLine(5, 6); safeLine(6, 7); safeLine(7, 4);
                safeLine(0, 4); safeLine(1, 5); safeLine(2, 6); safeLine(3, 7);

                if (filled_) {
                    const ImColor base(color);
                    const ImU32 fill = ImColor(base.Value.x, base.Value.y, base.Value.z, 0.2f);
                    auto safeFace = [&](int a, int b, int c, int d) {
                        if (sc[a].z > 1.f && sc[b].z > 1.f && sc[c].z > 1.f && sc[d].z > 1.f) {
                            ImVec2 pts[4] = { s2d[a], s2d[b], s2d[c], s2d[d] };
                            dl->AddConvexPolyFilled(pts, 4, fill);
                        }
                        };
                    safeFace(0, 1, 2, 3); safeFace(4, 5, 6, 7);
                    safeFace(0, 1, 5, 4); safeFace(1, 2, 6, 5);
                    safeFace(2, 3, 7, 6); safeFace(3, 0, 4, 7);
                }
            }

            if (tracers_) {
                const auto cs = OtherUtils::WorldToScreenPointCorrected(camera, eb.center);
                ImVec2 raw = { cs.x, screenH - cs.y };
                if (cs.z <= 0.1f) { raw.x = playerFrom.x * 2.f - raw.x; raw.y = playerFrom.y * 2.f - raw.y; }
                const bool onScreen = cs.z > 0.1f && raw.x >= 0 && raw.x <= screenW && raw.y >= 0 && raw.y <= screenH;
                DrawAnimatedTracer(dl, playerFrom, onScreen ? raw : ClampToScreenEdge(playerFrom, raw, screenW, screenH), color);
            }

            float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
            bool hasScreen = false;
            for (int k = 0; k < 8; ++k) {
                if (sc[k].z <= 1.f) continue;
                minX = (std::min)(minX, s2d[k].x); minY = (std::min)(minY, s2d[k].y);
                maxX = (std::max)(maxX, s2d[k].x); maxY = (std::max)(maxY, s2d[k].y);
                hasScreen = true;
            }

            if (hasScreen && font) {
                const auto cs = OtherUtils::WorldToScreenPointCorrected(camera, eb.center);
                const ImVec2 cr = { cs.x, screenH - cs.y };
                if (cs.z <= 0.1f || cr.x < 0 || cr.x > screenW || cr.y < 0 || cr.y > screenH) return;

                const float dist = std::sqrt(
                    (eb.center.x - playerPos.x) * (eb.center.x - playerPos.x) +
                    (eb.center.y - playerPos.y) * (eb.center.y - playerPos.y) +
                    (eb.center.z - playerPos.z) * (eb.center.z - playerPos.z));

                char label[256];
                std::snprintf(label, sizeof(label), XS("%s [%.1fm]"), name.c_str(), dist);

                const auto tsz = font->CalcTextSizeA(fontSize, FLT_MAX, 0, label);
                const float tx = (minX + maxX) * 0.5f - tsz.x * 0.5f;
                const float ty = maxY + 4.f;
                if (tx < 0 || ty < 0 || tx + tsz.x > screenW || ty + tsz.y > screenH) return;

                dl->AddText(font, fontSize, { tx + 1, ty + 1 }, IM_COL32(0, 0, 0, 190), label);
                dl->AddText(font, fontSize, { tx, ty }, IM_COL32(255, 255, 255, 255), label);
            }
        }))
    {
        return;
    }
}

void EspFeature::OnShutdown()
{
    DestroyAllSpirits();
    RestoreHighlight();
    SetEnabled(false);
}
