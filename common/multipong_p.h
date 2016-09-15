#pragma once


#ifndef JKN_CONFIG_DEBUG
#   define JKN_CONFIG_DEBUG 0
#endif

#if JKN_CONFIG_DEBUG
#   define JKN_ASSERT(expr, format, ...)                                \
            do                                                          \
            {                                                           \
                if (!(expr))                                            \
                {                                                       \
                      printf("hello");                                  \
                }                                                       \
            } while(0)
#endif

// include this after 
#include <jkn/jkn.h>