/*
 * error_ids.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_ERROR_IDS_H__
#define __PR_ERROR_IDS_H__


typedef void (*PR_ERROR_HANDLER_PROC)(PRenum errorID, const char* info);


//! No error
#define PR_ERROR_NONE                   0

//! Null pointer error
#define PR_ERROR_NULL_POINTER           1

//! Invalid argument error
#define PR_ERROR_INVALID_ARGUMENT       2

//! Invalid ID error
#define PR_ERROR_INVALID_ID             3

//! Invalid state error
#define PR_ERROR_INVALID_STATE          4

//! Index out of bounds error
#define PR_ERROR_INDEX_OUT_OF_BOUNDS    5

//! Argument mismatch error
#define PR_ERROR_ARGUMENT_MISMATCH      6

//! Missing plugin error
#define PR_ERROR_MISSING_PLUGIN         7

//! Unexpected end-of-file error
#define PR_ERROR_UNEXPECTED_EOF         8

//! Fatal error (occures only in debug mode)
#define PR_ERROR_FATAL                  9


#endif
