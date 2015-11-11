#include "ocr.h"
#include "ocr_relative_ptr.hpp"
#include "ocr_db_alloc.hpp"

using namespace Ocr;

class Grid2D {
    private:
        static double **_initGrid2D(int rows, int cols) {
            double **grid = ocrNewArray(double*, rows);
            for (int r=0; r<rows; r++) {
                grid[r] = ocrNewArray(double, cols);
            }
            return grid;
        }

    public:
        const size_t m_rows;
        const size_t m_cols;
        double *const *const m_grid;

        Grid2D(size_t rowDim, size_t colDim):
            m_rows(rowDim), m_cols(colDim), m_grid(_initGrid2D(rowDim, colDim))
        { }
};

#define ARENA_SIZE 1024

extern "C"
ocrGuid_t mainEdt(u32 paramc, u64* paramv, u32 depc, ocrEdtDep_t depv[]) {
    // Set up arena datablock
    void *arenaPtr;
    ocrGuid_t arenaGuid;
    ocrDbCreate(&arenaGuid, &arenaPtr, ARENA_SIZE, DB_PROP_NONE, NULL_GUID, NO_ALLOC);
    // Use arena as current allocator backing-datablock
    ocrAllocatorSetDb(arenaPtr, ARENA_SIZE, true);
    // Allocate a grid in the datablock
    Grid2D *grid = ocrNew(Grid2D, 10,10);
    // Is the grid in the datablock?
    assert((void*)grid == arenaPtr);
    // ...
    PRINTF("Hello from mainEdt()\n");
    ocrShutdown();
    return 0;
}
