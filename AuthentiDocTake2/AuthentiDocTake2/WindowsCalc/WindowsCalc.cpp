// WindowsCalc.cpp : Defines the exported functions for the static library.
//

#include "WindowsCalc.h"

// This is an example of an exported variable
int nWindowsCalc=0;

// This is an example of an exported function.
int fnWindowsCalc(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see WindowsCalc.h for the class definition
CWindowsCalc::CWindowsCalc()
{
    return;
}
