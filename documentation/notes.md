# Notes

## Memory Requirements

The strategy currently used in warpaffine can be summarized as follows:

* A custom allocator ([BrickAllocator](https://github.com/ZEISS/warpaffine/blob/main/libwarpaffine/BrickAllocator.h)) is used to manage memory for the input bricks, the output bricks and
 the compressed output brick (if applicable).
* For this allocator, we reserve a fixed amount of memory (the "memory budget") that is used for the entire processing.
* We define a high-water mark for the allocator - if memory usage is above this mark, then the reading the source is throttled.

By default, we reserve rougly the main memory size of the machine as the memory budget for the BrickAllocator.

Note that there is a minimal amount of memory required for the allocator, which is determined by the granularity of the processing
operations. This minimal amount is determined and checked for [here](https://github.com/ZEISS/warpaffine/blob/e1b47fa027f532fd6bfdbe56ad89fa0814b4f47b/libwarpaffine/configure.cpp#L42).  
It is the sum of the following:

* The memory size of the largest input tile, multiplied by the number of Z-slices of the input.
* The memory size of the largest output tile, multiplied by the number of Z-slices of the output.

The memory size of a tile is calculated by multiplying the tile's width and height with the number of bytes-per-pixel (e.g. 
2 for Gray16, and 1 for Gray8).   
The extent of an output tile can be specified with the command line parameter `-m` (or `--max-tile-extent`), where a default of 2048 is used.

The number of Z-slices of the output is determined by the number of Z-slices of the input, the geometric transformation applied to the input, and the ratio of the scaling factors in X/Y and Z direction.   
For the standard case (x-y scaling: 0.145 µm, z scaling: 0.200 µm) we have the following relationship:

| Mode of Operation                  | Z-slice Factor* |
|------------------------------------|---------|
| CoverGlassTransform                |  0.138  |
| CoverGlassTransform_and_xy_rotated |  0.138  |
| Deskew                             |  1      |

*Multiply source z-count by this factor to get destination z-count.

Example:
-------

Assume we have a source CZI with 1000 Z-slices, the pixel format is Gray16, and the largest tile in the source is 2048x400. We have a 
scaling where above table applies, and we want to use the default max tile extent of 2048.   
Then the minimal memory requirement is:

Input brick:  2048 × 400 × 2 (bytes per pixel) × 1000 (Z-slices) = 1,638,400,000 bytes ≈ 1.53 GB  
Output brick: 2048 × 2048 × 2 (bytes per pixel) × 1000 × 0.138 (Z-slice factor) = 1,157,627,904 bytes ≈ 1.08 GB

**Total: ≈ 2.60 GB**

