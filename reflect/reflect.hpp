#pragma once
#ifndef REFLECT_HPP
#define REFLECT_HPP

#include <type_traits>
#include <utility>
#include <cstddef>

namespace reflect {

template<typename T>
struct Tag {
    using type = T;
};
template<typename...A>
struct Attrs {};

namespace detail {

template<typename T, typename = void>
struct info {
    using cls = void;
    using type = T;
};

template <class C, typename T> auto get_memptr_type(T C::*) -> Tag<T>;
template <class C, typename T> auto get_memptr_class(T C::*) -> Tag<C>;
template <class C, typename T, typename...Args> auto get_memptr_type(T (C::*v)(Args...)) -> Tag<decltype(v)>;
template <class C, typename T, typename...Args> auto get_memptr_class(T (C::*)(Args...)) -> Tag<C>;

template<typename T>
struct info<T, std::enable_if_t<std::is_member_pointer_v<T>>> {
    using cls = typename decltype(detail::get_memptr_class(T{}))::type;
    using type = typename decltype(detail::get_memptr_type(T{}))::type;
};

template<typename Target>
auto do_extract(Attrs<>) -> Tag<void>;

template<typename Target, typename Head, typename...Tail>
auto do_extract(Attrs<Head, Tail...>) {
    if constexpr (std::is_base_of_v<Target, Head>) return Tag<Head>{};
    else return detail::do_extract<Target>(Attrs<Tail...>{});
}

} //detail

template<size_t i, auto v, typename...Attr>
struct Member {
    const char* name = "";
    static constexpr auto value = v;
    static constexpr auto id = i;
    using raw_type = decltype(v);
    using type = typename detail::info<raw_type>::type;
    using cls = typename detail::info<raw_type>::cls;
    static constexpr auto is_method = std::is_member_function_pointer_v<raw_type>;
    static constexpr auto is_field = std::is_member_object_pointer_v<raw_type>;
    template<typename T> static constexpr decltype(auto) get(T&& obj) noexcept {
        static_assert(is_field);
        return std::forward<T>(obj).*v;
    }
    template<typename T, typename...Args> static constexpr decltype(auto) call(T&& obj, Args&&...a) {
        static_assert(is_method);
        return (std::forward<T>(obj).*v)(std::forward<Args>(a)...);
    }
    template<typename A>
    static constexpr bool has_attr() {
        return (false || ... || std::is_base_of_v<A, Attr>);
    }
    using Attributes = Attrs<Attr...>;
    static constexpr Attributes get_attrs() {
        return {};
    }
};

template<typename T, typename = void>
struct is_reflected : std::false_type {};

template<typename T>
struct is_reflected<T, std::void_t<decltype(T::Reflection), typename T::Attributes>> : std::true_type {};

template<typename T>
constexpr bool is_reflected_v = is_reflected<T>::value;

template<typename Attr, typename T, typename = void>
struct extract {
    using type = typename decltype(detail::do_extract<Attr>(typename T::Attributes{}))::type;
};

template<typename Attr, typename T>
using extract_t = typename extract<Attr, T>::type;

} //reflect

#define REFLECT(Class, ...) \
private: \
    using _ = Class; \
    static constexpr size_t _memberIdBase = __COUNTER__; \
public: \
    using Attributes = ::reflect::Attrs<__VA_ARGS__>; \
    template<typename Fn> \
    static constexpr void Reflection(Fn _reflect)

#define MEMBER(f, ...) _reflect(reflect::Member<(__COUNTER__ - 1) - _::_memberIdBase, &_::f, ##__VA_ARGS__>{#f});

#endif // REFLECT_HPP
