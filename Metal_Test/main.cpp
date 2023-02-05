//
//  main.cpp
//  Metal_Test
//
//  Created by Francisco del Campo on 2/3/23.
//


#include <iostream>
#include <chrono>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

void array_init(int size, int *arr) {
    for(int i=0; i<size; i++) {
        arr[i]=i;
    }
}

void slow_addition(int size, int *inA, int *inB, int *inC) {
    for(int i = 0; i<size; i++) {
        inC[i] = inA[i] + inB[i];
    }
}

int main(int argc, const char * argv[]) {
    std::string str;
    std::cin >> str;
    int s = stoi(str);
    
    int *a = (int*)malloc(sizeof(int)*s);
    int *b = (int*)malloc(sizeof(int)*s);
    int *c = (int*)malloc(sizeof(int)*s);
    
    array_init(s, a);
    array_init(s, b);
    
    MTL::Device* device = MTLCreateSystemDefaultDevice();
    MTL::CommandQueue* commandQueue = device->newCommandQueue();
    MTL::Library* library = device->newDefaultLibrary();
    NS::String* functionName = NS::String::string("fast_addition", NS::ASCIIStringEncoding);
    MTL::Function* function = library->newFunction(functionName);
    NS::Error** error = NULL;
    MTL::ComputePipelineState* pipeline = device->newComputePipelineState(function, error);

    
    NS::UInteger nS = sizeof(int)*s;
    
    MTL::Buffer* bufA = device->newBuffer(a, nS, 0);
    MTL::Buffer* bufB = device->newBuffer(b, nS, 0);
    MTL::Buffer* sum = device->newBuffer(nS, 0);
    
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* commandEncoder = commandBuffer->computeCommandEncoder();
    
    commandEncoder->setComputePipelineState(pipeline);
    commandEncoder->setBuffer(bufA, 0, 0);
    commandEncoder->setBuffer(bufB, 0, 1);
    commandEncoder->setBuffer(sum, 0, 2);
    
    MTL::Size gridShape = MTL::Size(NS::UInteger(s), NS::UInteger(1), NS::UInteger(1));
    NS::UInteger max = pipeline->maxTotalThreadsPerThreadgroup();
    MTL::Size groupShape = MTL::Size(NS::UInteger(max), NS::UInteger(1), NS::UInteger(1));
    commandEncoder->dispatchThreads(gridShape, groupShape);
    commandEncoder->endEncoding();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    slow_addition(s, a, b, c);
    
    auto middle = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<float> duration1 = middle - start;
    std::cout << duration1.count() << std::endl;
    
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    int* d = (int*) sum->contents();
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration2 = end - middle;
    
    std::cout << duration2.count() << std::endl;
    
    free(bufA);
    free(bufB);
    free(sum);
    
    device->release();
    
    free(a);
    free(b);
    free(c);
    
    return 0;
}
