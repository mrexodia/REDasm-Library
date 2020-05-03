#pragma once

#include <type_traits>
#include <functional>
#include <sstream>
#include <iomanip>
#include <string>
#include <rdapi/types.h>

class BufferView;

class Utils
{
    public:
        Utils() = delete;
        static inline u8* relpointer(void* ptr, size_t offset) { return reinterpret_cast<u8*>(reinterpret_cast<u8*>(ptr) + offset); }
        static std::string hexString(BufferView* view);
        static std::string toString(const std::u16string& s);
        static std::string quoted(const std::u16string& s);
        static std::string quoted(const std::string& s);
        static std::string quotedSingle(const std::u16string& s);
        static std::string quotedSingle(const std::string& s);
        static std::string simplified(const std::u16string& s);
        static std::string simplified(std::string s);
        static void yloop(const std::function<bool()>& cb);

    public:
        template<typename T> static std::string number(T value, size_t base = 10, size_t width = 0, char fill = '\0');
        template<typename T> static std::string hex(T t, size_t bits = 0, bool withprefix = false);
};

template<typename T>
std::string Utils::number(T value, size_t base, size_t width, char fill)
{
    if((base == 10) && !width && !fill) return std::to_string(value);

    std::stringstream ss;

    if(base == 8) ss << std::oct;
    else if(base == 16) ss << std::hex;

    if(width) ss << std::setw(width);
    if(fill) ss << std::setfill(fill);

    if(sizeof(T) == 1)
    {
        if(std::is_signed<T>::value) ss << static_cast<typename std::make_signed<size_t>::type>(value);
        else ss << static_cast<size_t>(value);
    }
    else
        ss << value;

    return ss.str();
}

template<typename T>
std::string Utils::hex(T value, size_t bits, bool withprefix)
{
    std::stringstream ss;
    if(withprefix && (value > (std::is_signed<T>::value ? 9 : 9u))) ss << "0x";

    ss << std::uppercase << std::hex;

    if(bits > 0) ss << std::setfill('0') << std::setw(bits / 4);

    if(std::is_signed<T>::value && value < 0) ss << "-" << (~value) + 1;
    else ss << value;

    return ss.str().c_str();
}