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
using SkiaSharp;

namespace AuthentiDocTake2.WindowsShared
{
    public static class OpenCVComputeAPIhandle
    {
        public static long handle = -1;
    }
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
            ImageProperties imgPropOut = new ImageProperties();
            imgPropOut.outAllocatedSize = inputImage.pitchBytes * inputImage.height;
            IOpenCVComputeAPI.GetOutBuf(OpenCVComputeAPIhandle.handle, inputImage.imageBuf, imgPropOut.outAllocatedSize, ref imgPropOut.outRealSize, ref imgPropOut.compressionType, ref imgPropOut.subCompressionType,
                 ref imgPropOut.width, ref imgPropOut.height, ref imgPropOut.bitDepth);
            IntPtr pString = ITessApiSignatures.APIRect(handle, inputImage.imageBuf, inputImage.bytesPerPixel, inputImage.pitchBytes, 0, 0, inputImage.width, inputImage.height);
            var strRes = Marshal.PtrToStringAnsi(pString);
             
            //var b = new SharpDX.DXGI.Surface();
            //var a = new SharpDX.Direct2D1.RenderTarget(,,);
            //GetBoxes(inputImage);
            // SkiaSharp.SKRectI rect = new SKRectI(0,0,100,100);
            return strRes;
        }
        //void a()
        //{
        //    {
        //        RenderForm form = new RenderForm("TEST APP");
        //        Factory fact = new Factory();
        //        Device dev = new Device(fact.Adapters[0]);
        //        SharpDX.DXGI.SwapChain chain = new SwapChain(fact, dev, new SwapChainDescription()
        //        {
        //            BufferCount = 1,
        //            Flags = SwapChainFlags.AllowModeSwitch,
        //            IsWindowed = true,
        //            ModeDescription = new ModeDescription(800, 600, new Rational(1, 60), Format.B8G8R8A8_UNorm),
        //            OutputHandle = form.Handle,
        //            SampleDescription = new SampleDescription(1, 0),
        //            SwapEffect = SwapEffect.Discard,
        //            Usage = Usage.RenderTargetOutput
        //        });
                
        //        Xamarin.Forms.Platform.WinRT.ImageConverter a = new Xamarin.Forms.Platform.WinRT.ImageConverter();
        //        a.Convert()
        //        Texture2D backbuffer = Texture2D.FromSwapChain<Texture2D>(chain, 0);
        //        RenderTargetView rtv = new RenderTargetView(dev, backbuffer);
        //        SharpDX.Direct3D11.DeviceContext context = dev.ImmediateContext;

        //        Surface surf = chain.GetBackBuffer<Surface>(0);
        //        Factory2 fact2 = new Factory2(FactoryType.SingleThreaded, DebugLevel.Information);
        //        // Query for the adapter and more advanced DXGI objects.
        //        var dxgiDevice2 = dev.QueryInterface<SharpDX.DXGI.Device>();

        //        // Get the default Direct2D device and create a context.
        //        var d2DDevice = new SharpDX.Direct2D1.Device(dxgiDevice2);
        //        var _d2DContext = new SharpDX.Direct2D1.DeviceContext(d2DDevice, DeviceContextOptions.None);

        //        _d2DContext.PrimitiveBlend = PrimitiveBlend.SourceOver;

        //        var properties = new BitmapProperties1(new PixelFormat(Format.B8G8R8A8_UNorm, SharpDX.Direct2D1.AlphaMode.Premultiplied),
        //            96, 96, BitmapOptions.Target | BitmapOptions.CannotDraw);

        //        var _targ = new Bitmap1(_d2DContext, chain.GetBackBuffer<Surface>(0), properties);
        //        _d2DContext.Target = _targ;

        //    }
        //}
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