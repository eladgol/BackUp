using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;
using SharpDX.WIC;
using System.Reflection;
using Windows.Storage;
//using SkiaSharp;
namespace AuthentiDocTake2.WindowsShared
{
    class WindowsSignalProcess : SignalProcess  
    {
        public IntPtr handle = IntPtr.Zero;
        public System.Int32 res = -3;
        public WindowsSignalProcess()
        {
            handle = ITessApiSignatures.BaseApiCreate();
            res = ITessApiSignatures.BaseApiInit3(handle, Constants.TesseractDataSetPath, "eng");
        }
        public async Task<string> GetText(InputImage inputImage)
        {
            IntPtr pString = ITessApiSignatures.APIRect(handle, inputImage.imageBuf, inputImage.bytesPerPixel, inputImage.pitchBytes, 0, 0, inputImage.width, inputImage.height);
            var strRes = Marshal.PtrToStringAnsi(pString);
            //GetBoxes(inputImage);


            return strRes;
        }
        //public /*async Task<List<SKRectI>>*/ void GetBoxes(InputImage inputImage)
        //{
        //    int x0, y0, x1, y1;
            //List<SKRectI> boxesList = new List<SKRectI>();
            //int res = 0;
            //while (res == 0)
            //{
                //res = ITessApiSignatures.PageIteratorBoundingBox(handle, PageIteratorLevel.Symbol, out x0, out y0, out x1, out y1);
                //SKRectI rect = new SKRectI(x0, y0, x1, y1);
                //boxesList.Add(rect);
            //}
            //return boxesList;
        //}
        public async Task<InputImage> PreProcess(InputImage inputImage)
        {
            throw new NotImplementedException();
        }


    }
}