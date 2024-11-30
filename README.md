# silly_reflect
Reflection with template lambda visitor!
# Example
```cpp
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
```