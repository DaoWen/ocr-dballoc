#ifndef _OCR_DB_ALLOC_HPP_
#define _OCR_DB_ALLOC_HPP_

#include <cstddef>
#include <cassert>
#include <memory>

namespace Ocr {
    namespace SimpleDbAllocator {

        struct AllocatorState {
            ptrdiff_t offset;
        };

        struct DbArenaHeader {
            size_t size;
            ptrdiff_t offset;
        };

        class DatablockAllocator {
            private:
                char *const m_dbBuf;
                DbArenaHeader *const m_info;

            public:
                constexpr DatablockAllocator(void):
                    m_dbBuf(nullptr), m_info(nullptr) { }

                DatablockAllocator(void *dbPtr):
                    m_dbBuf((char*)dbPtr), m_info((DbArenaHeader*)dbPtr) { }

                AllocatorState saveState(void) {
                    return { m_info->offset };
                }

                void restoreState(AllocatorState state) {
                    m_info->offset = state.offset;
                }

                inline void *allocateAligned(size_t size, int alignment) const {
                    assert(m_info != nullptr && "Uninitialized allocator");
                    const ptrdiff_t offset = m_info->offset;
                    ptrdiff_t start = (offset + alignment - 1) & (-alignment);
                    m_info->offset = start + size;
                    assert(m_info->offset <= m_info->size
                           && "Datablock allocator overflow");
                    return (void*)&m_dbBuf[start];
                }

                // FIXME - I don't know if these alignment checks are sufficient,
                // especially if we do weird things like allocate a char[N]
                // so we have N bytes to store some struct or something.
                inline void *allocate(size_t size, size_t count) const {
                    assert(size > 0);
                    if (size == 1) {
                        return allocateAligned(1*count, 1);
                    }
                    else if (size <= 4) {
                        return allocateAligned(size*count, 4);
                    }
                    else if (size <= 8) {
                        return allocateAligned(size*count, 8);
                    }
                    else {
                        return allocateAligned(size*count, 16);
                    }
                }

                inline void *allocate(size_t size) const {
                    return allocate(size, 1);
                }

        };

        DatablockAllocator &ocrAllocatorGet(void);
        void ocrAllocatorSetDb(void *dbPtr);
        void ocrAllocatorDbInit(void *dbPtr, size_t dbSize);
    }

    /////////////////////////////////////////////
    // Arena management

    using Arena = SimpleDbAllocator::DatablockAllocator;

    static inline void InitializeArena(void *dbPtr, size_t dbSize) {
        SimpleDbAllocator::ocrAllocatorDbInit(dbPtr, dbSize);
    }

    static inline void SetCurrentArena(void *dbPtr) {
        SimpleDbAllocator::ocrAllocatorSetDb(dbPtr);
    }

    static inline Arena &GetCurrentArena(void) {
        return SimpleDbAllocator::ocrAllocatorGet();
    }

    template <typename T>
    static inline T &GetArenaRoot(void *dbPtr) {
        typedef SimpleDbAllocator::DbArenaHeader HT;
        HT *header = (HT*) dbPtr;
        return * (T*) &header[1];
    }

    /////////////////////////////////////////////
    // Ocr::New

    template <typename T, typename ...Ts>
    static inline T* NewIn(SimpleDbAllocator::DatablockAllocator arena, Ts&&... args) {
        auto mem = arena.allocate(sizeof(T));
        return new (mem) T(std::forward<Ts>(args)...);
    }

    template <typename T, typename ...Ts>
    static inline T* New(Ts&&... args) {
        SimpleDbAllocator::DatablockAllocator arena = Ocr::SimpleDbAllocator::ocrAllocatorGet();
        return NewIn<T, Ts...>(arena, std::forward<Ts>(args)...);
    }

    /////////////////////////////////////////////
    // Constructor helper for Ocr::NewArray

    template <typename T, typename NoInit = void>
    struct TypeInitializer {
        static inline void init(T& target) {
            new (&target) T();
        }
    };

    template <typename T>
    struct TypeInitializer<T, typename std::enable_if<std::is_scalar<T>::value>::type> {
        static inline void init(T&) { }
    };

    /////////////////////////////////////////////
    // Ocr::NewArray

    template <typename T>
    static inline T* NewArrayIn(SimpleDbAllocator::DatablockAllocator arena, size_t count) {
        T* data = reinterpret_cast<T*>(arena.allocate(sizeof(T), count));
        for (size_t i=0; i<count; i++) {
            TypeInitializer<T>::init(data[i]);
        }
        return data;
    }

    template <typename T>
    static inline T* NewArray(size_t count) {
        SimpleDbAllocator::DatablockAllocator arena = Ocr::SimpleDbAllocator::ocrAllocatorGet();
        return NewArrayIn<T>(arena, count);
    }

}

#endif /* _OCR_DB_ALLOC_HPP_ */
