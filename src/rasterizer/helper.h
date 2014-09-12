/*
 * helper.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_HELPER_H__
#define __PR_HELPER_H__


#define PR_SAFE_FREE(m) \
    if ((m) != NULL)    \
    {                   \
        free(m);        \
        m = NULL;       \
    }

#define PR_ZERO_MEMORY(m) memset(&m, 0, sizeof(m))


#endif
