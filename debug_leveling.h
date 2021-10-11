#define DEBUG_LVL 2

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
#endif

#if DEBUG_LVL >= 1
#define WarningProccessing(warning_code)                          \
WarningProccessing_(warning_code, __PRETTY_FUNCTION__, __LINE__)
#endif


#define ErrorsProccessing(error_code)                             \
ErrorsProccessing_(error_code, __PRETTY_FUNCTION__, __LINE__)
