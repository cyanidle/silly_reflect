#include "reflect/reflect.hpp"

// This test is compile-time
// Look at link errors to see type names

struct Base {};
struct Attr : Base {};
struct Attr1 {};
struct Attr2 {};

struct Data {
    int a[1];
    int b[2];
    int c[3];

    void method();
    virtual void virt_method();
    virtual void pure_method() = 0;

    REFLECT(Data, Attr) {
        MEMBER(a, Attr, int)
        MEMBER(b, Attr, float)
        MEMBER(c, Attr, bool)
        MEMBER(method, Attr, double)
        MEMBER(virt_method, Attr, long)
        MEMBER(pure_method, Attr, char)   
    }
};

template<typename...A>
extern size_t check_attrs(reflect::Attrs<A...>);

template<typename T>
void check_value(T&);

void test(Data* object) {
    Data::Reflection([&](auto f){
        using F = decltype(f);
        using A = reflect::extract_t<Base, F>;
        static_assert(!std::is_void_v<A>);
        static_assert(std::is_same_v<A, Attr>);
        check_attrs(f.get_attrs());
    });
}

int main(int argc, char *argv[])
{
    test(nullptr);
    return 0;
}

