using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
namespace AuthentiDocTake2.WindowsShared
{
    class WindowsThreading : IThreadingCrossPlatform
    {
        [DllImport("kernel32.dll", EntryPoint = "GetCurrentThreadId", CallingConvention = CallingConvention.Winapi,
            SetLastError = true, CharSet = CharSet.Ansi, BestFitMapping = false, ThrowOnUnmappableChar = true)]
        static extern uint GetCurrentNativeThreadId();
        public uint GetCurrentThreadID()
        {
            return GetCurrentNativeThreadId();
        }
    }
}
