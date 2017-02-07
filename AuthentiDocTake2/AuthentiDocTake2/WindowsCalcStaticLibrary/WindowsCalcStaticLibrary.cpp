// WindowsCalcStaticLibrary.cpp : Defines the exported functions for the static library.
//

#include "WindowsCalcStaticLibrary.h"
#include "C:\\Code\\MyCode\\WindowsOpenCVComputeDLL\\WindowsOpenCVComputeDLL\\WindowsOpenCVComputeDLL.h"
// This is an example of an exported variable
int nWindowsCalcStaticLibrary=0;

// This is an example of an exported function.
int fnWindowsCalcStaticLibrary(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see WindowsCalcStaticLibrary.h for the class definition
CWindowsCalcStaticLibrary::CWindowsCalcStaticLibrary()
{
	CWindowsOpenCVComputeDLL a;
	a.test();
    return;
}
