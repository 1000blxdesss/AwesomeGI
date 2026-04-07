#pragma once
// =================================
//  Don't pay attention, old trash
//  Don't pay attention, old trash
// ===============================

#include <cstddef>
#include <cstdint>
#include <utility>

namespace xor_detail {

    constexpr uint32_t file_hash(const char* s) {
        uint32_t h = 0x811c9dc5u;
        while (*s)
            h = (h ^ static_cast<uint8_t>(*s++)) * 0x01000193u;
        return h;
    }

    constexpr uint32_t seed(uint32_t line, uint32_t counter, uint32_t fhash) {
        uint32_t h = 0x811c9dc5u;
        h = (h ^ fhash) * 0x01000193u;
        h = (h ^ line) * 0x01000193u;
        h = (h ^ counter) * 0x01000193u;
        h = (h ^ (counter >> 8)) * 0x01000193u;
        return h;
    }

    constexpr uint8_t keyByte(uint32_t seed, size_t index) {
        uint32_t s = seed;
        for (size_t i = 0; i <= index; ++i)
            s = s * 1664525u + 1013904223u;
        return static_cast<uint8_t>(s >> 16);
    }

    template <typename CharT, size_t N, uint32_t Seed>
    class XorString {
    public:
        constexpr XorString(const CharT(&str)[N])
            : encrypted_{}
        {
            for (size_t i = 0; i < N; ++i) {
                auto ch = static_cast<uint8_t>(str[i]);
                encrypted_[i] = static_cast<CharT>(ch ^ keyByte(Seed, i));
            }
        }

        const CharT* decrypt() const {
            static CharT buf[N];
            for (size_t i = 0; i < N; ++i) {
                auto ch = static_cast<uint8_t>(encrypted_[i]);
                buf[i] = static_cast<CharT>(ch ^ keyByte(Seed, i));
            }
            return buf;
        }

        operator const CharT* () const { return decrypt(); }

    private:
        CharT encrypted_[N];
    };

} 

#define XS(str) \
    (::xor_detail::XorString<char, sizeof(str), \
        ::xor_detail::seed(__LINE__, __COUNTER__, ::xor_detail::file_hash(__FILE__))>(str).decrypt())

#define XSW(str) \
    (::xor_detail::XorString<wchar_t, sizeof(str)/sizeof(wchar_t), \
        ::xor_detail::seed(__LINE__, __COUNTER__, ::xor_detail::file_hash(__FILE__))>(str).decrypt())
