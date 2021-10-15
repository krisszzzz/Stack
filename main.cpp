
#include "Stack.h"

void printer(const int* to_pr)
{
    PrintToLog("%d", *to_pr);
}
//Stack_T(int**)


Stack_T(int)

void RunUnitTests(int* array, int array_size, void (*printer)(const int*));
int main()
{

    FILE* log = fopen("log_file.txt", "w");
    RememberLogFile(log);

    int arr[100] = {};  // Create an array to test stack
    RunUnitTests(arr, 35, printer);

}


void RunUnitTests(int* array, int array_size, void (*printer)(const int*))
{



     for(int arr_index = 0; arr_index < array_size; ++arr_index) // write numbers from 1 to 40 into the array
        array[arr_index] = arr_index + 1;

    stack_int integer_stack = {};
    CtorStack_int(&integer_stack, 10, printer);     // Check the Ctor and Dtor works. Also check what happen when ctor and dtor multiple called
    CtorStack_int(&integer_stack);                // We allow you to reuse your after destruction, but use Ctor to correct work
    //DtorStack_int(&integer_stack);                // Multiple Ctor calling is bad, but the program guarantees that it will continue
    DtorStack_int(&integer_stack);                // its work, but you will lose access to the data of the previous constructed stack
    CtorStack_int(&integer_stack, 10);            // Multiple Dtor calling will close your program with error

    for(int stack_size_count = 0; stack_size_count < array_size; ++stack_size_count) // push array_size numbers from the array onto the stack
        StackPush_int(&integer_stack, &array[stack_size_count]);

    IsEmptyStack_int        (&integer_stack);           // Test all stack functions. Warning: don't use GeneralInfoStack_int, use SafeGeneralInfoStack_int instead
    ValidateStack_int       (&integer_stack);          // For description of all function watch the documantion
    AssertNullptrStack_int  (&integer_stack);     //
    GeneralInfoStack_int    (&integer_stack);       //
    SafeGeneralInfoStack_int(&integer_stack);

    int test_count = 0;
    //integer_stack.size = 4; // Trying to change the stack size will cause assertion failed, but only in DEBUG_LVL 2

    while(test_count < array_size)
    {
        int last_stack_element = StackPop_int(&integer_stack);  //Mini test that shows how the protection system against stack underflow
        StackPop_int(&integer_stack);
        printf("%d", last_stack_element);
        StackPush_int(&integer_stack, &last_stack_element);
        test_count++;
    }
    DtorStack_int(&integer_stack);

}

