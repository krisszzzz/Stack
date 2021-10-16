

#ifndef STACK_INCLUDED

#define STACK_INCLUDED

#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include "log.h"
#include "debug_leveling.h"

#define MAKE_STACK_INVARIANT(elem_type, condition)                 \
if(!(condition)) {                                                 \
    GeneralInfoStack_##elem_type(stack_t);                         \
    ERROR_PROCCESSING(kStackHashIsInccorect);                      \
    return kErroredStack;                                          \
}

enum Warnings
{
	kStackUnderflow,
    kInccorectCapacityInputToCtor,
	kNotInitializedStack,
    kAlreadyConstructed
};
enum Errors
{
	kMemoryAllocationError = -13,
    kStackDataNullptr,
    kStackDataLeftCanaryIsInccorect,
    kStackRightLeftCanaryIsInccorect,
    kStackDataHashIsInccorect,
    kStackSizeBiggerThanCapacity,
    kStackSizeIsNegative,
    kStackCapacityIsInccorect,
    kStackHashIsInccorect,
    kStackLeftCanaryIsInccorect,
    kStackRightCanaryIsInccorect,
	kNullptrStack
};

const int     kErroredStack      = -1;
const int     kMinCapacity       = 8;
const int     kOffsetOnDeleting  = 3;
const __int64 kLeftCanaryValue   = 0xDEDAD & 0xDEDBAD;
const __int64 kRightCanaryValue  = 0xDED32 & 0xDED64;
const size_t  kCanarySize        = sizeof(__int64);
const size_t  kHashSize          = sizeof(unsigned __int64);

ON_DEBUG_LVL_1(
void             Warning_Proccessing_(const int warning_code, const char* function_name,
                                      const int line,         const char* file_name);
)

void             Error_Proccessing_  (const int error,        const char* funct, 
                                      const int line,         const char* flie_name);
int              Max                 (const int first,        const int second);
void*            recalloc            (void* block,            size_t elem_count, size_t elem_size);
unsigned __int64 RSHash              (const char* test,       size_t obj_size);
void             SetDataCanary       (char* data,             size_t size_of_data);
unsigned __int64 CalculateDataHash   (char* data,             size_t size_of_data);
void             ResetDataRightCanary(char* data,             size_t size_of_data);


#define Stack_T(elem_type)                                                                                     \
struct stack_##elem_type {																					   \
                                                                                                               \
    ON_DEBUG_LVL_1(                                                                                            \
    __int64           left_canary;                                                                             \
    )                                                                                                          \
                                                                                                               \
    elem_type*        data;																				       \
    ssize_t           size;																					   \
    ssize_t           capacity;																				   \
                                                                                                               \
    ON_DEBUG_LVL_1(                                                                                            \
    __int64           right_canary;                                                                            \
    )                                                                                                          \
    ON_DEBUG_LVL_2(                                                                                            \
    unsigned __int64  hash;                                                                                    \
    )                                                                                                          \
};                                                                                                             \
                                                                                                               \
                                                                                                               \
int IsNullptrStack_##elem_type (const stack_##elem_type * stack_t)                                             \
{                                                                                                              \
    if(stack_t == nullptr) {                                                                                   \
        ERROR_PROCCESSING(kNullptrStack);                                                                      \
        return 1;                                                                                              \
    }                                                                                                          \
    else                                                                                                       \
        return 0;                                                                                              \
}                                                                                                              \
                                                                                                               \
void (*SetPrinter_##elem_type(void (*printer)(const elem_type* to_print) = nullptr))(const elem_type* to_print)\
{                                                                                                              \
    static void (*stat_printer)(const elem_type* to_print) = nullptr;                                          \
    if(printer != nullptr) {                                                                                   \
        stat_printer = printer;                                                                                \
    }                                                                                                          \
                                                                                                               \
    return stat_printer;                                                                                       \
}                                                                                                              \
                                                                                                               \
                                                                                                               \
void GeneralInfoStack_##elem_type (const stack_##elem_type* stack_t,                                           \
                                     void (*printer)(const elem_type* to_print) = nullptr)                     \
{                                                                                                              \
    void (*ctor_printer)(const elem_type* to_print) = SetPrinter_##elem_type(printer);                         \
    if(IsNullptrStack_##elem_type(stack_t))                                                                    \
        return;                                                                                                \
                                                                                                               \
                                                                                                               \
    PrintToLog("-------------------------------------------------------------------------------\n");           \                                                                                                            
    PrintToLog("Stack info:\n stack address: %p, stack type: %s, stack size: %d, "                             \
                 "stack capacity: %d, stack data address: %p\n", stack_t, #elem_type, stack_t->size,           \
                                                                 stack_t->capacity,   stack_t->data);          \
                                                                                                               \
    for(int elem_of_data = 0; elem_of_data < stack_t->size; ++elem_of_data)                                    \
    {                                                                                                          \
        const elem_type* current_element_address = (const elem_type*)((char*)stack_t->data +                   \
                                                    ON_DEBUG_LVL_1( + kCanarySize) +                           \
                                                    elem_of_data * sizeof(elem_type));                         \
        PrintToLog("&[%d] == %p\n", elem_of_data, current_element_address);                                    \
        if(ctor_printer != nullptr) {                                                                          \
            PrintToLog  ("*[%d] == ", elem_of_data);                                                           \
            ctor_printer(current_element_address);                                                          \
            PrintToLog  ("\n");                                                                             \
        }                                                                                                   \
                                                                                                            \
    }                                                                                                       \
                                                                                                            \
}                                                                                                           \
                                                                                                            \
int IsInvalidStack_##elem_type(const stack_##elem_type* stack_t)				                            \
{                                                                                                           \
    if(IsNullptrStack_##elem_type(stack_t))                                                                 \
        return kErroredStack;                                                                               \
                                                                                                            \
    ON_DEBUG_LVL_1(                                                                                         \
         MAKE_STACK_INVARIANT(                                                                              \
            elem_type,                                                                                      \
            stack_t->left_canary == kLeftCanaryValue                                                        \
        )                                                                                                   \
        MAKE_STACK_INVARIANT(                                                                               \
            elem_type,                                                                                      \
            stack_t->right_canary == kRightCanaryValue                                                      \
        )                                                                                                   \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
    MAKE_STACK_INVARIANT(                                                                                   \
        elem_type,                                                                                          \
        stack_t->hash == RSHash((char*)stack_t, sizeof(stack_##elem_type) - kHashSize)                      \
    )                                                                                                       \
    )                                                                                                       \
     MAKE_STACK_INVARIANT(                                                                                  \
        elem_type,                                                                                          \
        stack_t->size <= stack_t->capacity                                                                  \
    )                                                                                                       \
    MAKE_STACK_INVARIANT(                                                                                   \
        elem_type,                                                                                          \
        stack_t->data != nullptr                                                                            \
    )                                                                                                       \
    MAKE_STACK_INVARIANT(                                                                                   \
        elem_type,                                                                                          \
        stack_t->size >= 0                                                                                  \
    )                                                                                                       \
    MAKE_STACK_INVARIANT(                                                                                   \
        elem_type,                                                                                          \
        stack_t->capacity >= kMinCapacity                                                                   \
    )                                                                                                       \
    ON_DEBUG_LVL_1(                                                                                         \
        size_t all_elements_size = stack_t->capacity * sizeof(elem_type);                                   \
                                                                                                            \
        MAKE_STACK_INVARIANT(                                                                               \
            elem_type,                                                                                      \
            *(__int64*)stack_t->data == kLeftCanaryValue                                                    \
        )                                                                                                   \
        char* stack_data_right_canary = (char*)stack_t->data + all_elements_size + kCanarySize;             \
        MAKE_STACK_INVARIANT(                                                                               \
            elem_type,                                                                                      \
            *(__int64*)stack_data_right_canary == kRightCanaryValue                                         \
        )                                                                                                   \
    )                                                                                                       \
                                                                                                            \
    ON_DEBUG_LVL_2(                                                                                         \
        char* data_hash = (char*)stack_t->data + all_elements_size + STACK_USE_CANARY * kCanarySize;        \
                                                                                                            \
        MAKE_STACK_INVARIANT(                                                                               \
            elem_type,                                                                                      \
            *(unsigned __int64*)data_hash == CalculateDataHash((char*) stack_t->data,                       \
                                                               all_elements_size)                           \
        )                                                                                                   \
    )                                                                                                       \
                                                                                                            \
    return 0;                                                                                               \                                                                                                        
}                                                                                                           \
                                                                                                            \
void SafeGeneralInfoStack_##elem_type(const stack_##elem_type* stack_t,                                     \
                                      void (*printer)(const elem_type* to_print) = nullptr)                 \
{                                                                                                           \
    if(IsInvalidStack_##elem_type(stack_t) == kErroredStack)                                                \
        return;                                                                                             \
    GeneralInfoStack_##elem_type(stack_t, printer);                                                         \
}                                                                                                           \
                                                                                                            \
void CtorStack_##elem_type(stack_##elem_type* stack_t, ssize_t capacity = kMinCapacity,					    \
                            void (*printer)(const elem_type* to_print) = nullptr)                           \
{                                                                                                           \
    if(IsNullptrStack_##elem_type(stack_t))                                                                 \
        return;                                                                                             \
    ON_DEBUG_LVL_1(                                                                                         \
        if(stack_t->left_canary == kLeftCanaryValue || stack_t->right_canary == kRightCanaryValue) {        \
            WARNING_PROCCESSING(kAlreadyConstructed);                                                       \
            return;                                                                                         \
        }                                                                                                   \
    )                                                                                                       \
                                                                                                            \
    ON_DEBUG_LVL_1(                                                                                         \
        if(capacity < 0) {                                                                                  \
            WARNING_PROCCESSING(kInccorectCapacityInputToCtor);                                             \
        }                                                                                                   \
                                                                                                            \
		stack_##elem_type null_initialized = {0};                                                           \
		if(memcmp(stack_t, &null_initialized, sizeof(elem_type)) != 0) {                                    \
			WARNING_PROCCESSING(kNotInitializedStack);                                                      \
        }                                                                                                   \
                                                                                                            \
		*stack_t = null_initialized; 																		\
    )                                                                                                       \
                                                                                                            \
    int to_alloc_capacity        = Max(kMinCapacity, capacity);                                             \
    size_t all_elements_size     = to_alloc_capacity * sizeof(elem_type);                                   \
    elem_type* data              = nullptr;                                                                 \
                                                                                                            \
	stack_t->capacity            = to_alloc_capacity;                                                       \
	stack_t->size                = 0;               														\
    data                         = (elem_type*)calloc(all_elements_size + STACK_PROT * kCanarySize,         \
                                    sizeof(char));                                                          \
                                                                                                            \
    if(data == nullptr) {                                                                                   \
        ERROR_PROCCESSING(kMemoryAllocationError);                                                          \
        return;                                                                                             \
    }                                                                                                       \
    ON_DEBUG_LVL_1(                                                                                         \
        SetDataCanary((char*)data, all_elements_size);                                                      \
        stack_t->left_canary  = kLeftCanaryValue;                                                           \
        stack_t->right_canary = kRightCanaryValue;                                                          \
    )                                                                                                       \
                                                                                                            \
    stack_t->data = data;                                                                                   \
                                                                                                            \
    ON_DEBUG_LVL_2(                                                                                                 \
        stack_t->hash                 = RSHash((char*)stack_t, sizeof(stack_##elem_type) - kHashSize);              \
        char* data_hash               = (char*)stack_t->data + all_elements_size + STACK_USE_CANARY * kCanarySize;  \
        *(unsigned __int64*)data_hash = CalculateDataHash((char*)data, all_elements_size);                          \
    )                                                                                                               \
                                                                                                            \
                                                                                                            \
                                                                                                            \
	if(IsInvalidStack_##elem_type(stack_t) == kErroredStack)                                                \
        return;                                                                                             \
    GeneralInfoStack_##elem_type(stack_t, printer);                                                         \
    PrintToLog("Construction are successfull\n");                                                           \
                                                                                                            \
                                                                                                            \
}																									        \
                    																				        \
void StackPush_##elem_type(stack_##elem_type* stack_t, elem_type* value)		                            \
{                                                                                                           \
    if(IsInvalidStack_##elem_type(stack_t) == kErroredStack)                                                \       
        return;                                                                                             \
    ON_DEBUG_LVL_1(                                                                                         \
        PrintToLog("Push function has been called by function - %s\n",                                      \
                      __PRETTY_FUNCTION__);                                                                 \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
        GeneralInfoStack_##elem_type(stack_t);                                                              \
    )                                                                                                       \
                                                                                                            \
    if(stack_t->size == stack_t->capacity) {                                                                \
        size_t all_elements_size = stack_t->capacity * sizeof(elem_type);                                   \
        elem_type* data = (elem_type*)recalloc(stack_t->data,                                               \
                                               2 * all_elements_size + STACK_PROT * kCanarySize,            \
                                               sizeof(char));                                               \
                                                                                                            \
        if(data == nullptr) {                                                                               \
            ERROR_PROCCESSING(kMemoryAllocationError);                                                      \
            return;                                                                                         \
        }                                                                                                   \
        ON_DEBUG_LVL_1(                                                                                     \
            ResetDataRightCanary((char*)data, all_elements_size);                                           \
            SetDataCanary((char*)data, 2 * all_elements_size);                                              \
        )                                                                                                   \
                                                                                                            \
        stack_t->capacity *= 2;                                                                             \
        stack_t->data      = data;                                                                          \
        ON_DEBUG_LVL_1(                                                                                     \
            PrintToLog("Memory reallocated from %d to %d\n", stack_t->capacity / 2,                         \
                                                             stack_t->capacity);                            \
        )                                                                                                   \
    }                                                                                                       \
                                                                                                            \
    elem_type* after_extreme_element_address = (elem_type*)((char*)stack_t->data                            \
                                                            ON_DEBUG_LVL_1( + kCanarySize) +                \
                                                            stack_t->size * sizeof(elem_type));             \
    *after_extreme_element_address = *value;                                                                \
    stack_t->size++;                                                                                        \
                                                                                                            \
    ON_DEBUG_LVL_2(                                                                                         \
       size_t all_elements_size = stack_t->capacity * sizeof(elem_type);                                    \
       stack_t->hash = RSHash((char*)stack_t, sizeof(stack_##elem_type) - kHashSize);                       \
       char* data_hash = (char*)stack_t->data + all_elements_size +                                         \
                               2 * kCanarySize;                                                             \
       *(unsigned __int64*)data_hash = CalculateDataHash((char*)stack_t->data,                              \
                                                          all_elements_size);                               \
    )                                                                                                       \
                                                                                                            \
    ON_DEBUG_LVL_1(                                                                                         \
        void (*push_printer)(const elem_type* to_print) = SetPrinter_##elem_type();                         \
        PrintToLog("The push the function has finished its work,"                                           \
                     " pushed element adress = %p, value = ", value);                                       \
                                                                                                            \
        if(push_printer == nullptr) {                                                                       \
            PrintToLog("UNKNOWN, because you did not pass "                                                 \
                         "the printer () function to the constructor\n");                                   \
        } else {                                                                                            \
            push_printer(value);                                                                            \
            PrintToLog("\n");                                                                               \
        }                                                                                                   \
                                                                                                            \
        ON_DEBUG_LVL_2(                                                                                     \
            GeneralInfoStack_##elem_type(stack_t);                                                          \
        )                                                                                                   \
    )                                                                                                       \
}                                                                                                           \
                                                                                                            \
int IsEmptyStack_##elem_type(stack_##elem_type* stack_t)										            \
{																									        \
    return (stack_t->size == 0) ? 1 : 0;																    \
}                                                                                                           \
                                                                                                            \
elem_type StackPop_##elem_type(stack_##elem_type* stack_t)					                                \
{                                                                                                           \
    if(IsInvalidStack_##elem_type(stack_t) == kErroredStack)                                                \
        {                                                                                                   \
            elem_type not_initialized_object;                                                               \
            return not_initialized_object;                                                                  \
        }                                                                                                   \
    void (*pop_printer)(const elem_type* to_print) = SetPrinter_##elem_type();					            \
                                                                                                            \
	if(!IsEmptyStack_##elem_type(stack_t)) {													            \
		if((stack_t->size <= stack_t->capacity / 2 - kOffsetOnDeleting) &&                                  \
            stack_t->capacity / 2 >= kMinCapacity) {                                                        \
            size_t all_elements_size = stack_t->capacity * sizeof(elem_type);                               \
			elem_type* data = (elem_type*)recalloc(stack_t->data,                                           \
                                                   all_elements_size / 2                                    \
                                                   + STACK_PROT * kCanarySize, sizeof(char));               \
                                                                                                            \
			if(data == nullptr) {                                                                           \
                ERROR_PROCCESSING(kMemoryAllocationError);                                                 \
			}                                                                                               \
			ON_DEBUG_LVL_1(                                                                                 \
                SetDataCanary((char*)data, all_elements_size / 2);                                          \
            )                                                                                               \
			                                                                                                \
			stack_t->capacity /= 2;                                                                         \
			stack_t->data = data;                                                                           \
        }                                                                                                   \
                                                                                                            \
        ON_DEBUG_LVL_1(                                                                                     \
            ssize_t element_in_data_index      = stack_t->size - 1;                                         \
            elem_type* extreme_element_address = (elem_type*)((char*)stack_t->data + kCanarySize +          \
                                                               element_in_data_index * sizeof(elem_type));  \
                                                                                                            \
			elem_type extreme_element_value    =  *(extreme_element_address);                               \
			*extreme_element_address           = {0};                                                       \
                                                                                                            \
            PrintToLog("Popped element address = %p, value = ",                                             \
                         extreme_element_address, extreme_element_value);                                   \
                                                                                                            \
            --stack_t->size;                                                                                \
                                                                                                            \
            if(pop_printer == nullptr) {                                                                    \
                PrintToLog("UNKNOWN, because you did not pass the printer"                                  \
                             " () function to the constructor\n");                                          \
            } else {                                                                                        \
                pop_printer(&extreme_element_value);                                                        \
                PrintToLog("\n");                                                                           \
            }                                                                                               \
                                                                                                            \
			ON_DEBUG_LVL_2(                                                                                 \
                size_t all_elements_size        = stack_t->capacity * sizeof(elem_type);                    \
                stack_t->hash                   = RSHash((char*)stack_t, sizeof(stack_##elem_type) - kHashSize);\
                char* data_hash                 = (char*)stack_t->data + all_elements_size +                    \
                                                   STACK_USE_CANARY * kCanarySize;                              \
                *(unsigned __int64*) data_hash  = CalculateDataHash((char*)stack_t->data, all_elements_size);   \
                GeneralInfoStack_##elem_type(stack_t);                                                      \
            )                                                                                               \
			return extreme_element_value;                                                                   \
        )                                                                                                   \
                                                                                                            \
		return stack_t->data[--stack_t->size];                                                              \
                                                                                                            \
	} else {																								\
        ON_DEBUG_LVL_1(                                                                                     \
	    elem_type null_initialized = {0};																	\
	    WARNING_PROCCESSING(kStackUnderflow);                                                               \
		return null_initialized;																			\
        )                                                                                                   \
    }																										\
}																											\
void DtorStack_##elem_type(stack_##elem_type* stack_t)							                            \
{                                                                                                           \
    PrintToLog("Destructor call for stack: \n");                                                            \
    if(IsInvalidStack_##elem_type(stack_t) == kErroredStack)                                                \
        return;                                                                                             \
    GeneralInfoStack_##elem_type(stack_t);															        \
                                                                                                            \
    stack_t->size = stack_t->capacity = 0;                                                                  \
    free(stack_t->data);                                                                                    \
    stack_t->data = nullptr;																	            \
    ON_DEBUG_LVL_1(                                                                                         \
        stack_t->left_canary  = 0;                                                                          \
        stack_t->right_canary = 0;                                                                          \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
        stack_t->hash         = 0;                                                                          \
    )                                                                                                       \
                                                                                                            \
    PrintToLog("Object destructed, you can reuse it, please use constructor for this case\n");              \
}

#endif


/*!@file    Stack.h
   \brief The core of the program. Here is the stack template - a macros analogue of C ++ templates for the C programming language.
   @par   How to use?
   @note Instruction:
         1. When you start a project write Stack_T (elem_type) where elem_type is the type of object for which you need a stack.
         Also, do not forget that if you want to use the program already as ready-made and tested, then set the DEBUG_LEVEL value to 0,
         this will mean that the program is in the release state.
         2. After that, the program will build the necessary structure, which will be called struct_elem_type where elem_type is the type
         of the object for which the stack was created.
         3. In total, the program creates 9 functions: IsEmptyStack_elem_type, DtorStack_elem_type, StackPush_elem_type, StackPop_elem_type,
         CtorStack_elem_type, IsInvalidStack_elem_type, GeneralInfoStack_elem_type, IsNullptrStack_elem_type - where
         elem_type is the type of object for which you need stack.
         4. Write an inference function of your type to improve debugging
         5. Use these functions to work with the stack, and do not forget to call DtorStack_elem_type after work - that is, the stack destructor
Stack functions description: (Throughout we will assume that elem_type is the type of the element for which the stack was built.)
         6. for debugging see documentation for stack_macros.h
  @par   void CtorStack_elem_type(stack_elem_type* to_ctor, ssize_t capacity = 8,void (*printer)(elem_type* to_print) = nullptr)
  Constructor of stack, using a stack without a constructor is an error that can cause the program to close.
  @param[to_ctor] - This is the stack you need to build
  @param[capacity] - the capacity of the stack, that is, the number of elements that the stack can store. Note that the stack is dynamic, so you don't
                     need to worry about overflowing the stack. By default, the minimum stack size is 8
  @param[(*printer)] - a pointer to a function that you need to write yourself. This function should print your elem_type correctly, because the stack
                       doesn't know how to print an arbitrary type. If you do not specify this pointer, then the stack will simply deduce that it does
                       not know how to print your data type. How to write a function to output a printer? See description below
  @return - its a void-type function, so nothing will be returned
  @par  void (*SetPrinterelem_type(void(*printer)(elem_type* to_print) = nullptr))(elem_type* to_print)
Quite a complicated looking function, but it is only used to make the stack remember your printer function.
  @param[(*printer)] - a pointer to a function that you need to write yourself. This function should print your elem_type correctly, because the stack
                       doesn't know how to print an arbitrary type. If you do not specify this pointer, then the stack will simply deduce that it does
                       not know how to print your data type. How to write a function to output a printer? See description below
  @return              Returns the last written function printer of the form void printer (elem_type * to_print)
  @par int is_invalid_stack_elem_type(stack_elem_type* stack_t)
  A function that checks your stack for data validity. Mostly used by functions inside
  the stack (each stack function has a validation check for the stack itself), but you can use it if you need it, but be careful, because if the stack is wrong,
  then the program will exit
  @param[stack_t] - The stack to check
  @return - 0 - if stack correct, else the program will exit
  @par void GeneralInfoStackelem_type(const stack_elem_type* stack_t, void(*printer)(elem_type* to_print))
Function for displaying basic information about the stack: about size, about capacity, about the address of
the stack, about the address of the elements of the stack, about the values ​​of the stack, if there is a print function
Use this function wisely, because if the stack pointer is null, the program will close
  @param[stack_t] - The stack for which basic information is displayed.
  @param[(*printer)] -  It is inside this function that the stack calls SetPrinterelem_type, that is,
  in this function it sets up the print function, which is passed through the constructor as an argument to this function.
  @return  - its a void-type function, so nothing will be returned
  @par void IsNullptrStack##elem_typestack_elem_type(const stack_elem_type* stack_t)
  Checks stack_t if it is a null pointer, if yes, then the program is closed
  @param[stack_t] - a pointer to the stack, which is checked against a null pointer
  @return - its a void-type function, so nothing will be returned
  @par void DtorStackelem_type(stack_elem_type* stack_t)
  Sets size and capacity to zero and frees data. After the destructor, you can use the object, but be sure to call the constructor again
  @param[stack_t] - Stack to be destroyed
  @return - its a void-type function, so nothing will be returned
  @warning When debug level is 0, always initialize the stack before calling the constructor,
  otherwise the program will crash. If the debug level is 1 or 2, then it will be guaranteed that
  the program will correctly create the stack.
*/