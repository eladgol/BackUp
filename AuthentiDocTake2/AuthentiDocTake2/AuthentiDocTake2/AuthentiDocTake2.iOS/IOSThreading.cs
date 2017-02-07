using System;
using System.Collections.Generic;
using System.Text;

namespace AuthentiDocTake2.iOS
{
    class IOSThreading : IThreadingCrossPlatform
    {
        public uint GetCurrentThreadID()
        {
            return (uint)System.Threading.Thread.CurrentThread.ManagedThreadId;
        }
    }
}
