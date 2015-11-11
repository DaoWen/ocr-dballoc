#include <cstddef>
#include <cstdio>
#include <cassert>

using namespace std;

/**
 * This is our "relative pointer" class.
 * You should be able to use it pretty much just like a normal pointer.
 * However, you still need to be careful to only point to memory within
 * the same datablock. Nothing keeps you from creating a "relative pointer"
 * into another datablock, or even into the stack.
 */
template <typename T>
struct OcrOffsetPtr {

    ptrdiff_t offset;

    // offset of 1 is impossible since this is larger than 1 byte
    OcrOffsetPtr(): offset(1) {}

    void set(const T *other) {
        offset = (char*)other - (char*)this;
    }

    T *get() const {
        assert(offset != 1);
        char *target = (char*)this + offset;
        return (T*)target;
    }

    OcrOffsetPtr(const T *other) { set(other); }

    OcrOffsetPtr<T> &operator=(const T *other) {
        set(other);
        return *this;
    }

    T &operator*() { return *get(); }

    T *operator->() { return get(); }

    T &operator[](const int index) { return get()[index]; }

    operator T*() const { return get(); }

};

struct Y { long a, b, c; };

int main(int argc, char *argv[]) {
    int x;
    OcrOffsetPtr<int> px = &x;
    OcrOffsetPtr<Y> py;
    Y y;
    py = &y;
    assert(&x == &*px);
    printf("x:   %p %+ld %p\n", &x, px.offset, &*px);
    assert(&x+1 == px+1);
    printf("x+1: %p %+ld %p\n", &x+1, px.offset, px+1);
    assert(&y == &py[0]);
    printf("y:   %p %+ld %p\n", &y, py.offset, &py[0]);
    assert(&y.b == &py->b);
    printf("y.b: %p %+ld %p\n", &y.b, py.offset, &py->b);
    return 0;
}
