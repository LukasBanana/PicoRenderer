/*
 * helper.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_HELPER_H
#define PR_HELPER_H


#define PR_MALLOC(t)        (t*)malloc(sizeof(t))
#define PR_CALLOC(t, n)     (t*)calloc(n, sizeof(t))

#define PR_FREE(m)      \
    if ((m) != NULL)    \
    {                   \
        free(m);        \
        m = NULL;       \
    }

#define PR_ZERO_MEMORY(m)   memset(&m, 0, sizeof(m))


#endif
