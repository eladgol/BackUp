using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AuthentiDocTake2.WindowsShared
{
    class IMyOpenCVComputeImport
    {
        [DllImport(@"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x64\Debug\AppX\WindowsOpenCVComputeDLL.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "Test")]
        public static extern void test();

    }
}
