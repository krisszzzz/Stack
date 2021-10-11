#include "Stack.h"

///
ON_DEBUG_LVL_1(

void WarningProccessing_(const int warning_code, const char* function_name,
                                                 const int line)
{
    switch(warning_code)
		{
			case NOT_INITIALIZED_STACK:  {
			    PrintToLog("WARNING: NOT_INITIALIZED_STACK, your stack has been"
                             " automatically zeroed (only for debug level 1 and higher)\n");
				break;
			}
			case STACK_UNDERFLOW:  {
				PrintToLog("WARNING: STACK_UNDERFLOW, pop function will"
                             " return you zeroed stack (only on debug level 1 and higher)\n");
				break;
			}
            case INCCORECT_CAPACITY_INPUT_TO_CTOR:  {
                PrintToLog("WARNING: INCCORECT_CAPACITY_INPUT_TO_CTOR,"
                             " you passed the wrong capacity to the constructor (negative capacity)\n");
                break;
            }
            case ALREADY_CONSTRUCTED:  {
                PrintToLog("WARNING: ALREADY_CONSTRUCTED, your stack was already constructed, "
                             "you will lose access to your stack data that was already constructed\n");
                break;
            }
            default:  {
                PrintToLog("Warning code is incorrect, something went wrong\n");
                assert(!"Unknown warning code");
            }

		}
}
)

void ErrorsProccessing_(const int error_code, const char* function_name, const int line)
{

	PrintToLog("ErrorsProccessing caused by %s, on the line under the number %d\n", function_name, line);

    switch(error_code)
    {
        case STACK_DATA_NULLPTR:  {
			    PrintToLog("ERROR: STACK_DATA_NULLPTR, your stack data is nullptr\n");
			    assert(!"ERROR: STACK_DATA_NULLPTR, your stack data is nullptr\n");
        }
        case NULLPTR_STACK:  {
                PrintToLog("ERROR: NULLPTR_STACK, you passed to the function that mentioned before the nullptr-object\n");
                assert(!"ERROR: NULLPTR_STACK, you passed to the function that mentioned before the nullptr-object\n");
        }
        case MEMORY_ALLOCATION_ERROR:  {
                PrintToLog("ERROR: MEMORY_ALLOCATION_ERROR, something went wrong in memory allocation or reallocation\n");
                assert(!"ERROR: MEMORY_ALLOCATION_ERROR, something went wrong in memory allocation or reallocation\n");
        }
        case STACK_DATA_HASH_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_DATA_HASH_IS_INCCORECT, your stack data was corrupted, the program was stopped\n");
                assert(!"ERROR: STACK_DATA_HASH_IS_INCCORECT, your stack data was corrupted, the program was stopped\n ");
        }
        case STACK_DATA_LEFT_CANARY_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_DATA_LEFT_CANARY_IS_INCCORECT, stack left canary (for data) has been changed\n");
                assert(!"ERROR: STACK_DATA_LEFT_CANARY_IS_INCCORECT, stack left canary (for data) has been changed\n");
        }
        case STACK_DATA_RIGHT_CANARY_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_DATA_RIGHT_CANARY_IS_INCCORECT, stack right canary (for data) has been changed\n");
                assert(!"ERROR: STACK_DATA_RIGHT_CANARY_IS_INCCORECT, stack right canary (for data) has been changed\n");
        }
        case STACK_HASH_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_HASH_IS_INCCORECT, your stack was corrupted\n");
                assert("ERROR: STACK_HASH_IS_INCCORECT, your stack was corrupted\n");
        }
        case STACK_SIZE_IS_NEGATIVE: {
                PrintToLog("ERROR: STACK_SIZE_IS_NEGATIVE, stack size can't be negative\n");
                assert("ERROR: STACK_SIZE_IS_NEGATIVE, stack size can't be negative\n");
        }
        case STACK_SIZE_BIGGER_THAN_CAPACITY:  {
                PrintToLog("ERROR: STACK_SIZE_BIGGER_THAN_CAPACITY, stack size can't be bigger than capacity\n");
                assert("ERROR: STACK_SIZE_BIGGER_THAN_CAPACITY, stack size can't be bigger than capacity\n");
        }
        case STACK_RIGHT_CANARY_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_RIGHT_CANARY_IS_INCCORECT, your stack right canary has been changed\n");
                assert("ERROR: STACK_RIGHT_CANARY_IS_INCCORECT, your stack right canary has been changed\n");
        }
        case STACK_LEFT_CANARY_IS_INCCORECT:  {
                PrintToLog("ERROR: STACK_LEFT_CANARY_IS_INCCORECT, your stack left canary has been changed\n");
                assert(!"ERROR: STACK_LEFT_CANARY_IS_INCCORECT, your stack left canary has been changed\n");
        }
        default:  {
                PrintToLog("Error code is incorrect, something went wrong\n");
                assert(!"Unknown error code\n");
        }

    }
}
int Max(const int first, const int second)
{
    return (first <= second) ? second : first;
}

void* recalloc(void* block, size_t elem_count, size_t elem_size)
{
	return realloc(block, elem_count * elem_size);
}

ON_DEBUG_LVL_1(

void SetDataCanary(char* data, size_t size_of_data)
{
    *(__int64*)data = LEFT_CANARY_VALUE;
    data += size_of_data + CANARY_SIZE;
    *(__int64*)data = RIGHT_CANARY_VALUE;
}

void ResetDataRightCanary(char* data, size_t size_of_data)
{
    data += size_of_data + CANARY_SIZE;
    *(__int64*)data = 0;
}

)

ON_DEBUG_LVL_2(

unsigned __int64 CalculateDataHash(char* data, size_t size_of_data)
{
    return RSHash((char*)data, size_of_data + STACK_USE_CANARY * CANARY_SIZE);
}

)

/*
void ResetDataHash(char* data, size_t size_of_data)
{
    char* hash_address      = data + size_of_data + STACK_USE_CANARY * CANARY_SIZE;
    *(__int64*)hash_address = 0;
}
*/
unsigned __int64 RSHash(const char* test, size_t obj_size)                     //!--------------------------------------------------------------------------------------
{                                                                               //! 
    unsigned __int64 a = 63689;
    unsigned __int64 b = 378551;
    unsigned __int64 hash = 0;

    for (int i = 0; i < obj_size; ++i)
    {
        hash = hash * a + (unsigned __int64)test[i];
    }

    return hash;
}
