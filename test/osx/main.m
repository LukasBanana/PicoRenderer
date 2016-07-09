/*
 * test.c (OSX)
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#import <Cocoa/Cocoa.h>

#include <pico.h>


// --- global members --- //

PRobject context        = NULL;
PRobject frameBuffer    = NULL;
PRboolean isQuit        = PR_FALSE;


int main(int argc, char* argv[])
{
    return NSApplicationMain(argc, (const char**)argv);
}

