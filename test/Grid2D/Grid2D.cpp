#include "ocr.h"
#include "ocr_relative_ptr.hpp"
#include "ocr_db_alloc.hpp"

using namespace Ocr::SimpleDbAllocator;

template <typename T> using RelPtr = Ocr::OcrRelativePtr<T>;


class Grid2D {
    private:
        static RelPtr<double> *_initGrid2D(int rows, int cols) {
            RelPtr<double> *grid = ocrNewArray(RelPtr<double>, rows);
            for (int r=0; r<rows; r++) {
                grid[r] = ocrNewArray(double, cols);
            }
            return grid;
        }

    public:
        const size_t rows;
        const size_t cols;
        const RelPtr<const RelPtr<double>> grid;

        Grid2D(size_t rowDim, size_t colDim):
            rows(rowDim), cols(colDim), grid(_initGrid2D(rowDim, colDim))
        {
            int n = 0;
            for (int r=0; r<rows; r++) {
                for (int c=0; c<cols; c++) {
                    grid[r][c] = n++ / 10.0;
                }
            }
        }

        inline double at(int row, int col) const {
            assert(row >= 0 && col >= 0 && "Negative index");
            assert(row < rows && col < cols && "Index out of bounds");
            return grid[row][col];
        }
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
    Grid2D &grid = *ocrNew(Grid2D, 10,10);
    // Is the grid in the datablock?
    assert((void*)&grid == arenaPtr);
    // ...
    PRINTF("Item at grid[5][6] = %.1f\n", grid.at(5,6));
    ocrShutdown();
    return 0;
}
