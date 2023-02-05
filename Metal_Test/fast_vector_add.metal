//
//  fast_vector_add.metal
//  Metal_Test
//
//  Created by Francisco del Campo on 2/3/23.
//

#include <metal_stdlib>
using namespace metal;

kernel void fast_addition(constant float *arr1        [[ buffer(0) ]],
                                      constant float *arr2        [[ buffer(1) ]],
                                      device   float *resultArray [[ buffer(2) ]],
                                               uint   index [[ thread_position_in_grid ]]) {
    resultArray[index] = arr1[index] + arr2[index];
}
