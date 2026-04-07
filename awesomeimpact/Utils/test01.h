#pragma once
// =================================
// Don't pay attention, old trash
// Don't pay attention, old trash
// ==============================

#include <cstdint>
#include <cstddef>
#include <intrin.h>

#ifndef _WINDEF_
typedef void* PVOID;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef wchar_t WCHAR;
#endif

namespace lazy_detail {


    constexpr uint32_t kFnvBasis = 0x811C9DC5u;
    constexpr uint32_t kFnvPrime = 0x01000193u;

    constexpr char toLowerAscii(char c) {
        return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
    }

    constexpr uint32_t fnv1a(const char* str, uint32_t hash = kFnvBasis) {
        return (*str == '\0') ? hash : fnv1a(str + 1, (hash ^ static_cast<uint8_t>(*str)) * kFnvPrime);
    }


    constexpr uint32_t fnv1a_ci(const char* str, uint32_t hash = kFnvBasis) {
        return (*str == '\0') ? hash
            : fnv1a_ci(str + 1, (hash ^ static_cast<uint8_t>(toLowerAscii(*str))) * kFnvPrime);
    }


    struct UNICODE_STRING_LI {
        USHORT Length;
        USHORT MaximumLength;
        WCHAR* Buffer;
    };

    struct LIST_ENTRY_LI {
        LIST_ENTRY_LI* Flink;
        LIST_ENTRY_LI* Blink;
    };

    struct LDR_DATA_TABLE_ENTRY_LI {
        LIST_ENTRY_LI InLoadOrderLinks;
        LIST_ENTRY_LI InMemoryOrderLinks;
        LIST_ENTRY_LI InInitializationOrderLinks;
        void* DllBase;
        void* EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING_LI FullDllName;
        UNICODE_STRING_LI BaseDllName;
    };

    struct PEB_LDR_DATA_LI {
        ULONG Length;
        ULONG Initialized;
        void* SsHandle;
        LIST_ENTRY_LI InLoadOrderModuleList;
    };

    struct PEB_LI {
        unsigned char pad[0x18];
        PEB_LDR_DATA_LI* Ldr;
    };


    inline PEB_LI* GetPeb() {
#if defined(_M_X64) || defined(__x86_64__)
        return reinterpret_cast<PEB_LI*>(__readgsqword(0x60));
#else
        return reinterpret_cast<PEB_LI*>(__readfsdword(0x30));
#endif
    }

    inline uint32_t fnv1a_wide_ci(const wchar_t* str, size_t len) {
        uint32_t hash = kFnvBasis;
        for (size_t i = 0; i < len; ++i) {
            char c = static_cast<char>(str[i]);
            if (c >= 'A' && c <= 'Z') c += ('a' - 'A');
            hash = (hash ^ static_cast<uint8_t>(c)) * kFnvPrime;
        }
        return hash;
    }

    inline uint32_t fnv1a_narrow(const char* str) {
        uint32_t hash = kFnvBasis;
        while (*str) {
            hash = (hash ^ static_cast<uint8_t>(*str)) * kFnvPrime;
            ++str;
        }
        return hash;
    }

    inline void* FindModuleByHash(uint32_t moduleHash) {
        PEB_LI* peb = GetPeb();
        if (!peb || !peb->Ldr) return nullptr;

        LIST_ENTRY_LI* head = &peb->Ldr->InLoadOrderModuleList;
        LIST_ENTRY_LI* curr = head->Flink;

        while (curr != head) {
            auto* entry = reinterpret_cast<LDR_DATA_TABLE_ENTRY_LI*>(curr);
            if (entry->BaseDllName.Buffer && entry->BaseDllName.Length > 0) {
                uint32_t hash = fnv1a_wide_ci(
                    entry->BaseDllName.Buffer,
                    entry->BaseDllName.Length / sizeof(WCHAR));
                if (hash == moduleHash)
                    return entry->DllBase;
            }
            curr = curr->Flink;
        }
        return nullptr;
    }

    inline void* FindExportByHash(void* moduleBase, uint32_t funcHash) {
        if (!moduleBase) return nullptr;

        auto* base = static_cast<unsigned char*>(moduleBase);

        auto dosHeader = *reinterpret_cast<uint32_t*>(base + 0x3C);
        auto* ntHeaders = base + dosHeader;

#if defined(_M_X64) || defined(__x86_64__)
        uint32_t exportDirRVA = *reinterpret_cast<uint32_t*>(ntHeaders + 0x88);
        uint32_t exportDirSize = *reinterpret_cast<uint32_t*>(ntHeaders + 0x8C);
#else
        uint32_t exportDirRVA = *reinterpret_cast<uint32_t*>(ntHeaders + 0x78);
        uint32_t exportDirSize = *reinterpret_cast<uint32_t*>(ntHeaders + 0x7C);
#endif

        if (exportDirRVA == 0) return nullptr;

        auto* exportDir = base + exportDirRVA;

        uint32_t numNames = *reinterpret_cast<uint32_t*>(exportDir + 0x18);
        uint32_t addrTableRVA = *reinterpret_cast<uint32_t*>(exportDir + 0x1C);
        uint32_t nameTableRVA = *reinterpret_cast<uint32_t*>(exportDir + 0x20);
        uint32_t ordTableRVA = *reinterpret_cast<uint32_t*>(exportDir + 0x24);

        auto* addrTable = reinterpret_cast<uint32_t*>(base + addrTableRVA);
        auto* nameTable = reinterpret_cast<uint32_t*>(base + nameTableRVA);
        auto* ordTable = reinterpret_cast<uint16_t*>(base + ordTableRVA);

        for (uint32_t i = 0; i < numNames; ++i) {
            const char* name = reinterpret_cast<const char*>(base + nameTable[i]);
            if (fnv1a_narrow(name) == funcHash) {
                uint32_t funcRVA = addrTable[ordTable[i]];
                if (funcRVA >= exportDirRVA && funcRVA < exportDirRVA + exportDirSize)
                    continue;
                return base + funcRVA;
            }
        }
        return nullptr;
    }

    inline void* ResolveImport(uint32_t moduleHash, uint32_t funcHash) {
        void* mod = FindModuleByHash(moduleHash);
        if (!mod) return nullptr;
        return FindExportByHash(mod, funcHash);
    }

}

#define LI_MODULE_HASH(mod) (::lazy_detail::fnv1a_ci(#mod ".dll"))
#define LI_FUNC_HASH(func) (::lazy_detail::fnv1a(#func))
#define LI_FN(mod, func) \
    ([&]() -> decltype(&func) { \
        static void* _cached = nullptr; \
        if (!_cached) \
            _cached = ::lazy_detail::ResolveImport(LI_MODULE_HASH(mod), LI_FUNC_HASH(func)); \
        return reinterpret_cast<decltype(&func)>(_cached); \
    }())
#define LI(mod, func) LI_FN(mod, func)
#define LI_RAW(mod, func) \
    ([&]() -> void* { \
        static void* _cached = nullptr; \
        if (!_cached) \
            _cached = ::lazy_detail::ResolveImport(LI_MODULE_HASH(mod), LI_FUNC_HASH(func)); \
        return _cached; \
    }())