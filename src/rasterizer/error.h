/*
 * error.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef PR_ERROR_H
#define PR_ERROR_H


#include "types.h"
#include "error_ids.h"


#define PR_ERROR(err)           _pr_error_set(err, __FUNCTION__)
#define PR_SET_ERROR_FATAL(msg) _pr_error_set(PR_ERROR_FATAL, msg)


void _pr_error_set(PRenum errorID, const char* info);
PRenum _pr_error_get();

//! Sets the error event handler.
void _pr_error_set_handler(PR_ERROR_HANDLER_PROC errorHandler);


#endif
