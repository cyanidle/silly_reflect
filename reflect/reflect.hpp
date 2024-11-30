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

template<typename T, typename...Attr>
struct Class {
    using type = T;
    static constexpr Attrs<Attr...> get_attrs() {
        return {};
    }
    template<typename A>
    static constexpr bool has_attr() {
        return (false || ... || std::is_base_of_v<A, Attr>);
    }
};

template<size_t id, auto v, typename...Attr>
struct Member {
    static constexpr auto value = v;
    using raw_type = decltype(v);
    using type = typename detail::info<raw_type>::type;
    using cls = typename detail::info<raw_type>::cls;
    static constexpr auto is_method = std::is_member_function_pointer_v<raw_type>;
    static constexpr auto is_field = std::is_member_object_pointer_v<raw_type>;
    template<typename T> static constexpr decltype(auto) get(T&& obj) noexcept {
        return std::forward<T>(obj).*v;
    }
    template<typename A>
    static constexpr bool has_attr() {
        return (false || ... || std::is_base_of_v<A, Attr>);
    }
    static constexpr Attrs<Attr...> get_attrs() {
        return {};
    }
};

template<typename T, typename...Ms>
struct Reflection {
    using type = typename T::type;
    static constexpr auto get_attrs() {
        return T::get_attrs();
    }
    template<typename A>
    static constexpr bool has_attr() {
        return T::template has_attr<A>();
    }
    template<typename Fn>
    static constexpr void for_each(Fn f) {
        (static_cast<void>(f(Ms{})), ...);
    }
};

template<typename T, typename = void>
struct is_reflected : std::false_type {};

template<typename T>
struct is_reflected<T, std::void_t<typename T::Reflection>> : std::true_type {};

template<typename T>
constexpr bool is_reflected_v = is_reflected<T>::value;

template<typename Attr, typename T, typename = void>
struct extract {
    using type = typename decltype(detail::do_extract<Attr>(T::get_attrs()))::type;
};

template<typename Attr, typename T>
struct extract<Attr, T, std::void_t<typename T::Reflection>> :
                                                               extract<Attr, typename T::Reflection>
{};

template<typename Attr, typename T>
using extract_t = typename extract<Attr, T>::type;

template<typename T, typename Fn, typename = std::enable_if_t<is_reflected_v<T>>>
constexpr void for_each(Fn f) {
    T::Reflection::for_each(f);
}

} //reflect

#define REFLECT(x, ...) \
using _ = typename x::type; \
    using Reflection = reflect::Reflection<x, ##__VA_ARGS__>;

#define CLASS(x, ...) reflect::Class<x, ##__VA_ARGS__>
#define MEMBER(f, ...) reflect::Member<__COUNTER__, &_::f, ##__VA_ARGS__>


#endif // REFLECT_HPP

