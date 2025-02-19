#pragma once

#include <stdint.h> // uintptr_t

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = 0

#define uINIT_FUNCTION_PTR(func_name) \
    union func_name##_union func_name = { .addr = 0 }

#define uTYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    union func_name##_union { \
        func_name##_ptr ptr; \
        uintptr_t addr; \
    }; \
    extern union func_name##_union func_name

#define uiTYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    uTYPEDEF_FUNCTION_PTR(ret_type, func_name, __VA_ARGS__); \
    uINIT_FUNCTION_PTR(func_name)
