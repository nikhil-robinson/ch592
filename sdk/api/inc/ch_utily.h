#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


#define MAP(x,in_min,in_max,out_min,out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)