# C++ interop for OCR

This library provides C++ functions and data structures that enable compatibility between C++ objects and OCR datablocks.

## Arena-based allocation for datablocks

We include a very basic arena-based allocator implementation for OCR datablocks.
Arenas support allocating new objects within the arena, and freeing the entire arena;
however, objects allocated within the arena cannot be individually freed.
The allocator functionality is included in `ocr_db_alloc.hpp`.

Before allocating a new persistent object for OCR
(one that persists within a datablock across multiple EDTs),
we need to specify which datablock is to be used as the arena.
The arena is stored in thread-local (or EDT-local) storage,
and thus the effect of setting an arena should be considered as constrained to the current EDT.

* `Ocr::InitializeArena(dbPtr, dbSize)` set up the bookkeeping in a datablock for use as an arena. Any data already contained in the datablock is now invalidated.
* `Ocr::SetCurrentArena(dbPtr)` set a dbPtr as the current implicit arena for the currently-executing EDT.
* `Ocr::GetCurrentArena()` returns the EDTs current arena object. This function is called by the allocator functions in order to get the datablock used for persistent storage.

We define a set of functions, with an iterface modeled after `std::make_shared`, for creating new objects that are allocated within the current EDT's arena datablock.

* `Ocr::New<T>(...)` replaces a call to `new T(...)`.
* `Ocr::NewArray<T>(n)` replaces a call to `new T[n]`.

## Relative pointers

Since OCR is free to move a datablock at any time
(as long as it is not currently acquired by a running EDT),
it is not possible to store absolute pointers within a datablock
to objects stored within that same datablock.
(Inter-datablock pointers are also illegal, but that is a separate issue.)

Instead, intra-datablock pointers must be encoded as *offsets*.
We provide the `Ocr::RelPtr<T>` class to simplify the use of intra-datablock pointers/offsets.
The `Ocr::RelPtr<T>` overrides many of the operators used with pointers,
making it straightforward to refactor code currently using primitive pointers.
If a field in a class is declared with type `T*`,
then that type should be changed to `Ocr::RelPtr<T>`.
Similarly, a field of type `T**`,
then that type should be changed to `Ocr::RelPtr<Ocr::RelPtr<T>>`,
which can equivalently be declared as `Ocr::NestedRelPtr<T,2>`,
where the 2 indicates two levels of `RelPtr` indirection.

Note that only pointers stored in objects that are persisted in datablocks
must be converted to relative pointers,
since they might be live across multiple EDTs.
It is fine to use absolute pointers for temporary values,
as long as those pointers do not escape the current EDT.
