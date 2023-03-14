#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "loader_interfaces.h"
#include <openxr/openxr.h>

static void print_api_layers();
static void PrintInstanceExtensions(char* p_APILayer);
static void CreateInstance();
bool xr_check(XrInstance instance, XrResult result, const char* format, ...);