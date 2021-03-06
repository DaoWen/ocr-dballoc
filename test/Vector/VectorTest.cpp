#include "ocr.h"
#include "ocr_vector.hpp"
#include <cstring>

using namespace Ocr::SimpleDbAllocator;

#define ARENA_SIZE 1024
#define VECTOR_SIZE 32

extern "C"
ocrGuid_t mainEdt(u32 paramc, u64* paramv, u32 depc, ocrEdtDep_t depv[]) {
    // Set up arena datablock
    void *arenaPtr;
    ocrGuid_t arenaGuid;
    ocrDbCreate(&arenaGuid, &arenaPtr, ARENA_SIZE, DB_PROP_NONE, NULL_GUID, NO_ALLOC);
    Ocr::InitializeArena(arenaPtr, ARENA_SIZE);
    // Use arena as current allocator backing-datablock
    Ocr::SetCurrentArena(arenaPtr);
    // Allocate a vector in the datablock
    Ocr::Vector<int> &v = *Ocr::New<Ocr::Vector<int>>(0, VECTOR_SIZE);
    // Is the vector in the datablock?
    void *vecAddr = &v;
    void *arenaEnd = (char*)arenaPtr + ARENA_SIZE;
    assert(arenaPtr <= vecAddr && vecAddr <= arenaEnd);
    // Add some numbers to the vector
    for (int i=0; i<10; i++) {
        v.push_back(i*2);
    }
    // Check the size
    PRINTF("Size = %d\n", v.size());
    assert(v.size() == 10);
    // Sum the entries
    {
        int total = 0;
        for (auto p=v.begin(); p!=v.end(); p++) {
            total += *p;
            PRINTF("%d ", *p);
        }
        PRINTF("\nTotal is %d\n", total);
        assert(total == 90);
    }
    // Erase a few entries
    v.erase(v.end()-1);
    v.erase(v.begin());
    v.erase(v.begin()+3);
    // Sanity check again
    PRINTF("Size after erasing is %d\n", v.size());
    assert(v.size() == 7);
    {
        int total = 0;
        const Ocr::Vector<int> &vc = v;
        for (int i=0; i<vc.size(); i++) {
            total += vc[i];
            PRINTF("%d ", vc[i]);
        }
        PRINTF("\nNew total is %d\n", total);
        assert(total == 64);
    }
    // Done
    ocrShutdown();
    return 0;
}
