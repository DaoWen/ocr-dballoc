#ifndef _OCR_RELATIVE_PTR_HPP_
#define _OCR_RELATIVE_PTR_HPP_

/**
 * This is our "relative pointer" class.
 * You should be able to use it pretty much just like a normal pointer.
 * However, you still need to be careful to only point to memory within
 * the same datablock. Nothing keeps you from creating a "relative pointer"
 * into another datablock, or even into the stack.
 */
template <typename T>
struct OcrRelativePtr {

    ptrdiff_t offset;

    // offset of 1 is impossible since this is larger than 1 byte
    OcrRelativePtr(): offset(1) {}

    void set(const T *other) {
        if (other == nullptr) {
            offset = 0;
        }
        else {
            offset = (char*)other - (char*)this;
        }
    }

    T *get() const {
        assert(offset != 1);
        if (offset == 0) return nullptr;
        else {
            char *target = (char*)this + offset;
            return (T*)target;
        }
    }

    OcrRelativePtr(const T *other) { set(other); }

    OcrRelativePtr<T> &operator=(const T *other) {
        set(other);
        return *this;
    }

    T &operator*() { return *get(); }

    T *operator->() { return get(); }

    T &operator[](const int index) { return get()[index]; }

    operator T*() const { return get(); }

    /* TODO - implement math operators, like increment and decrement */

};

#endif /* _OCR_RELATIVE_PTR_HPP_ */


