#include <iostream>
#include <memory>
#include <mutex>

namespace track_heap_usage{

struct heap_usage {
    static uint32_t total_space_used;
    static uint32_t total_space_freed;
    static std::mutex lock_alloc_obj;
    static std::mutex lock_dealloc_obj;

    static uint32_t getCurrentUsage(){
        std::lock_guard(track_heap_usage::heap_usage::lock_alloc_obj);
        std::lock_guard(track_heap_usage::heap_usage::lock_dealloc_obj);
        return total_space_used - total_space_freed;
    }
    static void printUsage(){
        std::cout << "Total memory Usage: " << total_space_used << " bytes" << std::endl;
        std::cout << "Total freed memory: " << total_space_freed << " bytes" << std::endl;
        std::cout << "Current Usage: " << getCurrentUsage() << " bytes" << std::endl;
    }
};

uint32_t heap_usage::total_space_freed;
uint32_t heap_usage::total_space_used;
std::mutex heap_usage::lock_alloc_obj;
std::mutex heap_usage::lock_dealloc_obj;
}

void* operator new(size_t size){
    {
        std::lock_guard(track_heap_usage::heap_usage::lock_alloc_obj);
        track_heap_usage::heap_usage::total_space_used += size;
    }
    return malloc(size);
}
void operator delete(void * memory, size_t size){
    free(memory);
    {
        std::lock_guard(track_heap_usage::heap_usage::lock_dealloc_obj);
        track_heap_usage::heap_usage::total_space_freed += size; 
    }
}
