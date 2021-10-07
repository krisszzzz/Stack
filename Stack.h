
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include "log.h"

#define DEBUG_LVL 2
#define malloc() DO_NOT_USE_MALLOC

#define STACK_USE_HASH   0x01
#define STACK_USE_CANARY 0x02


#if DEBUG_LVL == 2
#define ON_DEBUG_LVL_2(code)  code
#define ON_DEBUG_LVL_1(code)  code
#define STACK_PROT (STACK_USE_HASH | STACK_USE_CANARY)
#elif DEBUG_LVL == 1
#define ON_DEBUG_LVL_2(code)
#define ON_DEBUG_LVL_1(code)  code
#define STACK_PROT STACK_USE_CANARY
#else
#undef  STACK_USE_HASH
#undef  STACK_USE_CANARY
#define STACK_PROT 0
#define ON_DEBUG_LVL_2(code)
#define ON_DEBUG_LVL_1(code)
#define DATA_CANARY_OFFSET 0
#endif

#if DEBUG_LVL >= 1
#define DATA_CANARY_OFFSET 1
#define warning_processing(warning_code)                          \
warning_processing_(warning_code, __PRETTY_FUNCTION__, __LINE__)
#endif


#define errors_processing(error_code)                             \
errors_processing_(error_code, __PRETTY_FUNCTION__, __LINE__)

//#define realloc() DO_NOT_USE_REALLOC

enum WARNINGS
{
	STACK_UNDERFLOW,
    INCCORECT_CAPACITY_INPUT_TO_CTOR,
	NOT_INITIALIZED_STACK,
    ALREADY_CONSTRUCTED
};
enum ERRORS
{
	MEMORY_ALLOCATION_ERROR = -13,
    STACK_DATA_NULLPTR,
    STACK_DATA_LEFT_CANARY_IS_INCCORECT,
    STACK_DATA_RIGHT_CANARY_IS_INCCORECT,
    STACK_DATA_HASH_IS_INCCORECT,
    STACK_SIZE_BIGGER_THAN_CAPACITY,
    STACK_SIZE_IS_NEGATIVE,
    STACK_CAPACITY_IS_INCCORECT,
    STACK_HASH_IS_INCCORECT,
    STACK_LEFT_CANARY_IS_INCCORECT,
    STACK_RIGHT_CANARY_IS_INCCORECT,
	NULLPTR_STACK
};

const int     MIN_CAPACITY       = 8;
const int     OFFSET_ON_DELETING = 3;
const __int64 LEFT_CANARY_VALUE  = 0xDEDAD & 0xDEDBAD;
const __int64 RIGHT_CANARY_VALUE = 0xDED32 & 0xDED64;

ON_DEBUG_LVL_1(
void             warning_processing_(const int warning_code, const char* function_name,
                                     const int line);
)

void             errors_processing_(const int error, const char* funct, const int line);
int              max(const int first, const int second);
void*            recalloc(void* block, size_t elem_count, size_t elem_size);
unsigned __int64 rs_hash(const char* test, size_t obj_size);
void             set_data_canary(char* data, size_t size_of_data);
void             calculate_data_hash(char* data, size_t size_of_data);
// в general_info убрана проверка

/*!@file    Stack.h
   \brief The core of the program. Here is the stack template - a macro analogue of C ++ templates for the C programming language.
   @par   How to use?
   @note Instruction:
         1. When you start a project write stack template (elem_type) where elem_type is the type of object for which you need a stack.
         Also, do not forget that if you want to use the program already as ready-made and tested, then set the DEBUG_LEVEL value to 0,
         this will mean that the program is in the release state.
         2. After that, the program will build the necessary structure, which will be called struct_elem_type where elem_type is the type
         of the object for which the stack was created.
         3. In total, the program creates 9 functions: is_empty_stack_elem_type, dtor_stack_elem_type, stack_push_elem_type, stack_pop_elem_type,
         ctor_stack_elem_type, is_invalid_stack_elem_type, is_invalid_stack_elem_type, general_info_elem_type, assert_nullptr_stack_elem_type - where
         elem_type is the type of object for which you need stack.
         4. Write an inference function of your type to improve debugging
         5. Use these functions to work with the stack, and do not forget to call dtor_stack_elem_type after work - that is, the stack destructor
Stack functions description: (Throughout we will assume that elem_type is the type of the element for which the stack was built.)

  @par   void ctor_stack_elem_type(stack_elem_type* to_ctor, ssize_t capacity = 8,void (*printer)(elem_type* to_print) = nullptr)
  Constructor of stack, using a stack without a constructor is an error that can cause the program to close.
  @param[to_ctor] - This is the stack you need to build
  @param[capacity] - the capacity of the stack, that is, the number of elements that the stack can store. Note that the stack is dynamic, so you don't
                     need to worry about overflowing the stack. By default, the minimum stack size is 8
  @param[(*printer)] - a pointer to a function that you need to write yourself. This function should print your elem_type correctly, because the stack
                       doesn't know how to print an arbitrary type. If you do not specify this pointer, then the stack will simply deduce that it does
                       not know how to print your data type. How to write a function to output a printer? See description below
  @return - its a void-type function, so nothing will be returned
  @par  void (*set_printer_elem_type(void(*printer)(elem_type* to_print) = nullptr))(elem_type* to_print)
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

  @par void general_info_stack_elem_type(const stack_elem_type* stack_t, void(*printer)(elem_type* to_print))

Function for displaying basic information about the stack: about size, about capacity, about the address of
the stack, about the address of the elements of the stack, about the values ​​of the stack, if there is a print function
Use this function wisely, because if the stack pointer is null, the program will close
  @param[stack_t] - The stack for which basic information is displayed.
  @param[(*printer)] -  It is inside this function that the stack calls set_printer_elem_type, that is,
  in this function it sets up the print function, which is passed through the constructor as an argument to this function.
  @return  - its a void-type function, so nothing will be returned
  @par void assert_nullptr_stack_elem_type(const stack_elem_type* stack_t)
  Checks stack_t if it is a null pointer, if yes, then the program is closed
  @param[stack_t] - a pointer to the stack, which is checked against a null pointer
  @return - its a void-type function, so nothing will be returned
  @par void dtor_stack_elem_type(stack_elem_type* stack_t)
  Sets size and capacity to zero and frees data. After the destructor, you can use the object, but be sure to call the constructor again
  @param[stack_t] - Stack to be destroyed
  @return - its a void-type function, so nothing will be returned

  @warning When debug level is 0, always initialize the stack before calling the constructor,
  otherwise the program will crash. If the debug level is 1 or 2, then it will be guaranteed that
  the program will correctly create the stack.

*/



#define stack_template(elem_type)                                                                              \
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
                                                                                                               \
void assert_nullptr_stack_##elem_type(const stack_##elem_type * stack_t)                                       \
{                                                                                                              \
    if(stack_t == nullptr) {                                                                                   \
        errors_processing(NULLPTR_STACK);                                                                      \
    }                                                                                                          \
}                                                                                                              \
                                                                                                               \
void (*set_printer_##elem_type(void (*printer)(elem_type* to_print) = nullptr))(elem_type* to_print)           \
{                                                                                                              \
    static void (*stat_printer)(elem_type* to_print) = nullptr;                                                \
    if(printer != nullptr) {                                                                                   \
        stat_printer = printer;                                                                                \
    }                                                                                                          \
                                                                                                               \
    return stat_printer;                                                                                       \
}                                                                                                              \
                                                                                                               \
                                                                                                               \
void general_info_stack_##elem_type (const stack_##elem_type* stack_t,                                         \
                                     void (*printer)(elem_type* to_print) = nullptr)                           \
{                                                                                                              \
    void (*ctor_printer)(elem_type* to_print) = set_printer_##elem_type(printer);                              \
    assert_nullptr_stack_##elem_type(stack_t);                                                                 \
                                                                                                               \
                                                                                                               \
    print_to_log("Stack info: stack address = %p, stack type - %s, stack size = %d, "                          \
                 "stack capacity = %d, stack data = %p\n", stack_t, #elem_type, stack_t->size,                 \
                                                           stack_t->capacity,   stack_t->data);                \
                                                                                                               \
    for(int elem_of_data = 0; elem_of_data < stack_t->size; ++elem_of_data)                                    \
    {                                                                                                          \
        elem_type* current_element_address = (int*)((char*)stack_t->data +                                     \
                                                    DATA_CANARY_OFFSET * sizeof(__int64) +                     \
                                                    elem_of_data * sizeof(elem_type));                         \
        print_to_log("&[%d] == %p\n", elem_of_data, current_element_address);                               \
        if(ctor_printer != nullptr) {                                                                       \
            print_to_log("*[%d] == ", elem_of_data);                                                        \
            ctor_printer(current_element_address);                                                          \
            print_to_log("\n");                                                                             \
        }                                                                                                   \
                                                                                                            \
    }                                                                                                       \
                                                                                                            \
}                                                                                                           \
                                                                                                            \
void validate_stack_##elem_type(const stack_##elem_type* stack_t)					                        \
{                                                                                                           \
    assert_nullptr_stack_##elem_type(stack_t);                                                              \
                                                                                                            \
    ON_DEBUG_LVL_1(                                                                                         \
        if(stack_t->left_canary != LEFT_CANARY_VALUE) {                                                     \
            errors_processing(STACK_LEFT_CANARY_IS_INCCORECT);                                              \
        }                                                                                                   \
        if(stack_t->right_canary != RIGHT_CANARY_VALUE) {                                                   \
            errors_processing(STACK_RIGHT_CANARY_IS_INCCORECT);                                             \
        }                                                                                                   \
                                                                                                            \
    )                                                                                                       \
                                                                                                            \
    if(stack_t->size < 0) {                                                                                 \
        general_info_stack_##elem_type(stack_t);                                                            \
        errors_processing(STACK_SIZE_IS_NEGATIVE);                                                          \
    }                                                                                                       \
	if(stack_t->size > stack_t->capacity) {                                                                 \
        general_info_stack_##elem_type(stack_t);                                                            \
        errors_processing(STACK_SIZE_BIGGER_THAN_CAPACITY);                                                 \
    }                                                                                                       \
    if(stack_t->data == nullptr) {                                                                          \
        general_info_stack_##elem_type(stack_t);                                                            \
        errors_processing(STACK_DATA_NULLPTR);                                                              \
    }                                                                                                       \
    if(stack_t->capacity < MIN_CAPACITY) {                                                                  \
        general_info_stack_##elem_type(stack_t);                                                            \
        errors_processing(STACK_CAPACITY_IS_INCCORECT);                                                     \
    }                                                                                                       \
    ON_DEBUG_LVL_1(                                                                                         \
        if(*(__int64*)stack_t->data != LEFT_CANARY_VALUE) {                                                 \
            general_info_stack_##elem_type(stack_t);                                                        \
            errors_processing(STACK_DATA_LEFT_CANARY_IS_INCCORECT);                                         \
        }                                                                                                   \
        char* stack_data_right_canary = (char*)stack_t->data +                                              \
                                         stack_t->capacity * sizeof(elem_type) + sizeof(__int64);           \
                                                                                                            \
        if(*(__int64*)stack_data_right_canary != RIGHT_CANARY_VALUE) {                                      \
            general_info_stack_##elem_type(stack_t);                                                        \
            errors_processing(STACK_DATA_RIGHT_CANARY_IS_INCCORECT);                                        \
        }                                                                                                   \
                                                                                                            \
    )                                                                                                       \
                                                                                                            \
}                                                                                                           \
                                                                                                            \
void ctor_stack_##elem_type(stack_##elem_type* stack_t, ssize_t capacity = MIN_CAPACITY,					\
                            void (*printer)(elem_type* to_print) = nullptr)                                 \
{                                                                                                           \
    assert_nullptr_stack_##elem_type(stack_t);                                                              \
    ON_DEBUG_LVL_1(                                                                                         \
        if(stack_t->left_canary == LEFT_CANARY_VALUE || stack_t->right_canary == RIGHT_CANARY_VALUE) {      \
            warning_processing(ALREADY_CONSTRUCTED);                                                        \
            return;                                                                                         \
        }                                                                                                   \
    )                                                                                                       \
    ON_DEBUG_LVL_1(                                                                                         \
        if(capacity < 0) {                                                                                  \
            warning_processing(INCCORECT_CAPACITY_INPUT_TO_CTOR);                                           \
        }                                                                                                   \
                                                                                                            \
		stack_##elem_type null_initialized = {0};                                                           \
		if(memcmp(stack_t, &null_initialized, sizeof(elem_type)) != 0) {                                    \
			warning_processing(NOT_INITIALIZED_STACK);                                                      \
        }                                                                                                   \
                                                                                                            \
		*stack_t = null_initialized; 																		\
    )                                                                                                       \
                                                                                                            \
    int        to_alloc_capacity = max(MIN_CAPACITY, capacity);                                             \
    elem_type* data              = nullptr;                                                                 \
                                                                                                            \
	stack_t->capacity            = to_alloc_capacity;                                                       \
	stack_t->size                = 0;               														\
    data                         = (elem_type*)calloc(to_alloc_capacity * sizeof(elem_type) +               \
                                    STACK_PROT * sizeof(__int64), sizeof(char));                            \
                                                                                                            \
    if(data == nullptr) {                                                                                   \
        errors_processing(MEMORY_ALLOCATION_ERROR);                                                         \
    }                                                                                                       \
    ON_DEBUG_LVL_1(                                                                                         \
        set_data_canary((char*)data, to_alloc_capacity * sizeof(elem_type));                                \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
        stack_t->hash = rs_hash((char*)stack_t, sizeof(stack_##elem_type) - sizeof(__int64));               \
        calculate_data_hash((char*)data, to_alloc_capacity * sizeof(elem_type));                            \
    )                                                                                                       \
                                                                                                            \
    stack_t->data = data;                                                                                   \
    ON_DEBUG_LVL_1(                                                                                         \
        stack_t->left_canary  = LEFT_CANARY_VALUE;                                                          \
        stack_t->right_canary = RIGHT_CANARY_VALUE;                                                         \
    )                                                                                                       \
	validate_stack_##elem_type(stack_t);                                                                    \
    general_info_stack_##elem_type(stack_t, printer);                                                       \
    print_to_log("Construction are successfull\n");                                                         \
                                                                                                            \
}																									        \
                    																				        \
void stack_push_##elem_type(stack_##elem_type* stack_t, elem_type* value)		                            \
{                                                                                                           \
    validate_stack_##elem_type(stack_t);                                                                    \
    ON_DEBUG_LVL_1(                                                                                         \
        print_to_log("The push function has been called, name of the function - %s\n",                      \
                      __PRETTY_FUNCTION__);                                                                 \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
        general_info_stack_##elem_type(stack_t);                                                            \
    )                                                                                                       \
                                                                                                            \
    if(stack_t->size == stack_t->capacity) {                                                                \
        elem_type* data = (elem_type*)recalloc(stack_t->data,                                               \
                                               2 * stack_t->capacity + STACK_PROT * sizeof(__int64),        \
                                               sizeof(char));                                               \
                                                                                                            \
        if(data == nullptr) {                                                                               \
            free(stack_t->data);                                                                            \
            errors_processing(MEMORY_ALLOCATION_ERROR);                                                     \
        }                                                                                                   \
        stack_t->capacity *= 2;                                                                             \
        stack_t->data      = data;                                                                          \
        ON_DEBUG_LVL_1(                                                                                     \
            print_to_log("Memory reallocated from %d to %d\n", stack_t->capacity / 2,                       \
                                                               stack_t->capacity);                          \
        )                                                                                                   \
    }                                                                                                       \
                                                                                                            \
    elem_type* after_extreme_element_address = (elem_type*)((char*)stack_t->data +                          \
                                                            DATA_CANARY_OFFSET * sizeof(__int64) +          \
                                                            stack_t->size * sizeof(elem_type));             \
    *after_extreme_element_address = *value;                                                                \
                                                                                                            \
    stack_t->size++;                                                                                        \
    ON_DEBUG_LVL_2(                                                                                         \
    stack_t->hash = rs_hash((char*)stack_t, sizeof(stack_##elem_type) - sizeof(__int64));                   \
    calculate_data_hash((char*)stack_t->data, stack_t->capacity * sizeof(elem_type));                       \
    )                                                                                                       \
                                                                                                            \
    ON_DEBUG_LVL_1(                                                                                         \
        void (*push_printer)(elem_type* printer) = set_printer_##elem_type();                               \
        print_to_log("The push the function has finished its work,"                                         \
                     " pushed element adress = %p, value = ", value);                                       \
                                                                                                            \
        if(push_printer == nullptr) {                                                                       \
            print_to_log("UNKNOWN, because you did not pass "                                               \
                         "the printer () function to the constructor\n");                                   \
        } else {                                                                                            \
            push_printer(value);                                                                            \
            print_to_log("\n");                                                                             \
        }                                                                                                   \
                                                                                                            \
        ON_DEBUG_LVL_2(                                                                                     \
            general_info_stack_##elem_type(stack_t);                                                        \
        )                                                                                                   \
    )                                                                                                       \
}                                                                                                           \
                                                                                                            \
int is_empty_stack_##elem_type(stack_##elem_type* stack_t)											        \
{																									        \
    validate_stack_##elem_type(stack_t);                                                                    \
                                                                                                            \
    return (stack_t->size == 0) ? 1 : 0;																    \
}                                                                                                           \
                                                                                                            \
elem_type stack_pop_##elem_type(stack_##elem_type* stack_t)					                                \
{                                                                                                           \
    validate_stack_##elem_type(stack_t);                                                                    \
                                                                                                            \
    void (*pop_printer)(elem_type* to_print) = set_printer_##elem_type();					                \
                                                                                                            \
	if(!is_empty_stack_##elem_type(stack_t)) {														        \
		if((stack_t->size <= stack_t->capacity / 2 - OFFSET_ON_DELETING) &&                                 \
            stack_t->capacity / 2 >= MIN_CAPACITY) {                                                        \
                                                                                                            \
			elem_type* data = (elem_type*)recalloc(stack_t->data,                                           \
                                                   stack_t->capacity / 2 + STACK_PROT * sizeof(__int64),    \
                                                   sizeof(elem_type));                                      \
                                                                                                            \
			if(data == nullptr) {                                                                           \
                free(stack_t->data);                                                                        \
                errors_processing(MEMORY_ALLOCATION_ERROR);                                                 \
			}                                                                                               \
			stack_t->capacity /= 2;                                                                         \
			stack_t->data = data;                                                                           \
        }                                                                                                   \
                                                                                                            \
        ON_DEBUG_LVL_1(                                                                                     \
            ssize_t element_in_data_index      = stack_t->size - 1;                                         \
            elem_type* extreme_element_address = (elem_type*)((char*)stack_t->data +                        \
                                                               DATA_CANARY_OFFSET * sizeof(__int64) +       \
                                                               element_in_data_index * sizeof(elem_type));  \
                                                                                                            \
			elem_type extreme_element_value   =  *(extreme_element_address);                                \
			*extreme_element_address = {0};                                                                 \
                                                                                                            \
            print_to_log("Popped element address = %p, value = ",                                           \
                         extreme_element_address, extreme_element_value);                                   \
            --stack_t->size;                                                                                \
                                                                                                            \
            if(pop_printer == nullptr) {                                                                    \
                print_to_log("UNKNOWN, because you did not pass the printer"                                \
                             " () function to the constructor\n");                                          \
            } else {                                                                                        \
                pop_printer(&extreme_element_value);                                                        \
                print_to_log("\n");                                                                         \
            }                                                                                               \
                                                                                                            \
			ON_DEBUG_LVL_2(                                                                                 \
                stack_t->hash = rs_hash((char*)stack_t, sizeof(stack_##elem_type) - sizeof(__int64));       \
                calculate_data_hash((char*)stack_t->data, stack_t->capacity * sizeof(elem_type));           \
                general_info_stack_##elem_type(stack_t);                                                    \
            )                                                                                               \
			return extreme_element_value;                                                                   \
        )                                                                                                   \
                                                                                                            \
		return stack_t->data[--stack_t->size];                                                              \
                                                                                                            \
	} else {																								\
        ON_DEBUG_LVL_1(                                                                                     \
	    elem_type null_initialized = {0};																	\
	    warning_processing(STACK_UNDERFLOW);                                                                \
		return null_initialized;																			\
        )                                                                                                   \
    }																										\
}																											\
void dtor_stack_##elem_type(stack_##elem_type* stack_t)							                            \
{                                                                                                           \
    print_to_log("Destructor call for stack: \n");                                                          \
    validate_stack_##elem_type(stack_t);                                                                    \
    general_info_stack_##elem_type(stack_t);															    \
                                                                                                            \
    stack_t->size         = stack_t->capacity = 0;                                                          \
    stack_t->data         = nullptr;																	    \
    ON_DEBUG_LVL_1(                                                                                         \
     stack_t->left_canary  = 0;                                                                             \
    stack_t->right_canary = 0;                                                                              \
    )                                                                                                       \
    ON_DEBUG_LVL_2(                                                                                         \
    stack_t->hash         = 0;                                                                              \
    )                                                                                                       \
    free(stack_t->data);                                                                                    \
    print_to_log("Object destructed, you can reuse it, please use constructor for this case\n");            \
}
