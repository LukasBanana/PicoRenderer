/*
 * error_ids.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_ERROR_IDS_H__
#define __PR_ERROR_IDS_H__


typedef void (*PR_ERROR_HANDLER_PROC)(PRenum errorID, const char* info);


#define PR_ERROR_NONE                   0
#define PR_ERROR_NULL_POINTER           1
#define PR_ERROR_INVALID_ARGUMENT       2
#define PR_ERROR_INVALID_ID             3
#define PR_ERROR_INVALID_STATE          4
#define PR_ERROR_INDEX_OUT_OF_BOUNDS    5
#define PR_ERROR_ARGUMENT_MISMATCH      6
#define PR_ERROR_MISSING_PLUGIN         7


#endif
