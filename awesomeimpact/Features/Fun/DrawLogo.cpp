#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/Embedded/logintexture_blob.h"

#include <cstring>

namespace {

    bool g_logoApplied = false;
    Il2CppArray* g_byteTemplate = nullptr;

    inline Il2CppArray* CreateLogoBytes()
    {
        if (!g_byteTemplate)
        {
            auto* blackTex = UnityUtils::Texture2D_get_blackTexture();
            if (blackTex)
                g_byteTemplate = UnityUtils::ImageConversion_EncodeToPNG(blackTex);
        }
        if (!g_byteTemplate)
            return nullptr;

        return il2cppUtils::ArrayFromBytesL(
            g_byteTemplate, embedded_logo::data, embedded_logo::size);
    }

}

void (*Image_set_sprite_Orig)(void*, void*) = nullptr;
void Image_set_sprite_Hook(void* __this, void* value)
{
    if (!g_logoApplied && value)
    {
        auto* tex = UnityUtils::Sprite_get_texture(value);
        auto* bytes = CreateLogoBytes();

        if (tex && bytes && UnityUtils::ImageConversion_LoadImage(tex, bytes))
        {
            auto* data = il2cppUtils::ArrayDataPtr(bytes);
            if (data) std::memset(data, 0, embedded_logo::size);
            g_logoApplied = true;
        }
    }

    Image_set_sprite_Orig(__this, value);
}