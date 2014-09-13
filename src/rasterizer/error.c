/*
 * error.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "error.h"
#include "error_ids.h"


static PRenum _error = PR_ERROR_NONE;
static PR_ERROR_HANDLER_PROC _errorHandler = NULL;

void _pr_error_set(PRenum errorID, const char* info)
{
    _error = errorID;
    if (_errorHandler != NULL)
        _errorHandler(errorID, info);
}

PRenum _pr_error_get()
{
    return _error;
}

void _pr_error_set_handler(PR_ERROR_HANDLER_PROC errorHandler)
{
    _errorHandler = errorHandler;
}
