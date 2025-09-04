#include <type_traits>

template <typename T>
struct is_one_byte
{
    static constexpr bool value = (sizeof(T) == 1);
};

template <typename T>
inline constexpr bool is_one_byte_v = is_one_byte<T>::value;

template <typename T>
concept ByteType = std::is_same_v<T, std::byte> || (is_one_byte_v<T> && std::is_unsigned_v<T>);