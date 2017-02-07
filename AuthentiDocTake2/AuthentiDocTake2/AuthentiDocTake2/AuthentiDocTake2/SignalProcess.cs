using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading.Tasks;
//using SkiaSharp;

namespace AuthentiDocTake2
{
    public interface SignalProcess
    {
        Task<InputImage>  PreProcess(InputImage inputImage);
        Task<string> GetText(InputImage inputImage);
        //Task<List<SKRectI>> GetBoxes(InputImage inputImage);
    }
}
