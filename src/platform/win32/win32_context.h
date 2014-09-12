/*
 * context.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_ERROR_H__
#define __PR_ERROR_H__


#include "types.h"
#include "error_ids.h"


void _pr_error_set(PRenum errorID);
PRenum _pr_error_get();


#endif
