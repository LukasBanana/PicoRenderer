/*
 * error.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "error.h"
#include "error_ids.h"


static PRenum _error = PR_ERROR_NONE;

void _pr_set_error(PRenum errorID)
{
    _error = errorID;
}

PRenum _pr_get_error()
{
    return _error;
}
