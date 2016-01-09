#include "ocr_db_alloc.hpp"

#ifdef __APPLE__
// For some reason they don't support the standard C++ thread_local,
// but they *do* support the non-standard __thread extension for C.
#define thread_local __thread
#endif /* __APPLE__ */

namespace Ocr {
    namespace SimpleDbAllocator {
        static thread_local DatablockAllocator _localDbAllocator;

        DatablockAllocator &ocrAllocatorGet(void) {
            return _localDbAllocator;
        }

        void ocrAllocatorSetDb(void *dbPtr) {
            new (&_localDbAllocator) DatablockAllocator(dbPtr);
        }

        void ocrAllocatorDbInit(void *dbPtr, size_t dbSize) {
            DbArenaHeader *const info = (DbArenaHeader*) dbPtr;
            assert(dbSize >= sizeof(*info)
                   && "Datablock is too small for allocator");
            info->size = dbSize;
            info->offset = sizeof(*info);
        }
    }
}
