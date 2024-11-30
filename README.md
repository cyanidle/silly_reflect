# silly_reflect
Reflection with template lambda visitor!
# Example
```cpp
#include "reflect/reflect.hpp"
#include <iostream>


struct Base {};
struct Attr : Base {};
struct Attr1 {};
struct Attr2 {};

struct Data {
    int a[1];
    int b[2];
    int c[3];

    int method();
    virtual int virt_method();
    virtual int pure_method() = 0;

    REFLECT(Data, Attr) {
        MEMBER(a, Attr, int)
        MEMBER(b, Attr, float)
        MEMBER(c, Attr, bool)
        MEMBER(method, Attr, double)
        MEMBER(virt_method, Attr, long)
        MEMBER(pure_method, Attr, char)   
    }
};

void test(Data* object) {
    Data::Reflection([&](auto f){
        using Field = decltype(f);
        using Attrs = typename Field::Attributes;
        if constexpr (f.is_field) {
            std::cout << f.name << ": field value: " << f.get(*object) << std::endl;
        } else if constexpr (f.is_method) {
            std::cout << f.name << ": call() result: " << f.call(*object) << std::endl;
        }
    });
}


```
