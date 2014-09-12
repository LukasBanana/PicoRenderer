/*
 * pixel.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "pixel.h"

#include <limits.h>


PRushort _pr_pixel_write_depth(PRfloat z)
{
    return (PRushort)(z * (PRfloat)USHRT_MAX);
}

PRfloat _pr_pixel_read_depth(PRushort z)
{
    return ((PRfloat)z) / ((PRushort)USHRT_MAX);
}

