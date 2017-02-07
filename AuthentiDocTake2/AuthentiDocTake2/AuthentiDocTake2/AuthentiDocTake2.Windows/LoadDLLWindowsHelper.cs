using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Storage;
using System.Runtime.InteropServices;

namespace AuthentiDocTake2.WindowsShared
{
    static class LoadDLLWindowsHelper
    {
        [DllImport("kernel32", EntryPoint = "LoadLibrary", CallingConvention = CallingConvention.Winapi,
            SetLastError = true, CharSet = CharSet.Ansi, BestFitMapping = false, ThrowOnUnmappableChar = true)]
        private static extern IntPtr WindowsLoadLibrary(string dllPath);

        [DllImport("kernel32", EntryPoint = "FreeLibrary", CallingConvention = CallingConvention.Winapi,
            SetLastError = true, CharSet = CharSet.Ansi, BestFitMapping = false, ThrowOnUnmappableChar = true)]
        private static extern bool WindowsFreeLibrary(IntPtr handle);

        [DllImport("kernel32", EntryPoint = "GetProcAddress", CallingConvention = CallingConvention.Winapi,
            SetLastError = true)]
        private static extern IntPtr WindowsGetProcAddress(IntPtr handle, string procedureName);
        private static int WindowsGetLastError()
        {
            return Marshal.GetLastWin32Error();
        }
        public static void LoadDLL()
        {
            //   StorageFile dllFile = GetDLL(@"Assets\libtesseract304.dll").Result;
            //var handle = WindowsLoadLibrary(@"Assets\libtesseract304.dll");
            // C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x86\Debug\AppX\
            // C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\Debug\AppX\Assets\libtesseract304.dll
            //C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x86\Debug\AppX\Assets
             var handle = WindowsLoadLibrary(@"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x86\Debug\AppX\libtesseract304.dll");
            if (handle == IntPtr.Zero)
            {
                int res = WindowsGetLastError();
                System.Diagnostics.Debug.WriteLine("Failed loading " + res.ToString());
            }
            else
            {
                IntPtr funcP = WindowsGetProcAddress(handle, "TessBaseAPICreate");
                var TessBaseAPICreate = Marshal.GetDelegateForFunctionPointer<ITessApiSignatures.delegateBaseApiCreate>(funcP);
            }

        }
        static async Task<StorageFile> GetDLL(string libraryFiles)
        {
            var InstallationFolder = Package.Current.InstalledLocation;
            return await InstallationFolder.GetFileAsync(libraryFiles);
        }
    }
}
