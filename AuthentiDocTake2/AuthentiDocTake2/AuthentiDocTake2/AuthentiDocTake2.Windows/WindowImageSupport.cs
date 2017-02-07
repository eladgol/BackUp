using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using System.IO;
using Windows.UI.Xaml.Media.Imaging;
using Windows.Storage.Streams;
using Windows.Storage;
using SkiaSharp;
using AuthentiDocTake2;
using System.Runtime.InteropServices;
using System.Reflection;

namespace AuthentiDocTake2.WindowsShared
{
    class WindowsFileSupport : IFileSupport
    {
        
        public async Task<MemoryStream> FileToStream(string fileName)
        {
            StorageFile file = await StorageFile.GetFileFromPathAsync(fileName);
            //GetExecutingAssemblyPath.GetDirectoryName( Ass GetExecutingAssembly().Location);
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                return (MemoryStream)stream.AsStream();
            }
        }
         
    }
    public class IOpenCVComputeAPI
    {
        [DllImport(Constants.OpenCVComputeDLLName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CreateOCVCInstance")]
        public static extern long CreateOCVCInstance();
        [DllImport(Constants.OpenCVComputeDLLName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "DecompressImage")]
        public static extern int DecompressImage(long handle, IntPtr inBuf, int inSize, IntPtr outBuf, int outAllocatedSize, ref int outRealSize,
            ref int compressionType, ref int subCompressionType, ref int width, ref int height, ref int bitDepth);
        [DllImport(Constants.OpenCVComputeDLLName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "PostProc")]
        public static extern int PostProc(long handle, IntPtr outBuf, int outAllocatedSize, ref int outRealSize,
            ref int compressionType, ref int subCompressionType, ref int width, ref int height, ref int bitDepth);
        [DllImport(Constants.OpenCVComputeDLLName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetOutBuffer")]
        public static extern int GetOutBuf(long handle, IntPtr outBuf, int outAllocatedSize, ref int outRealSize,
            ref int compressionType, ref int subCompressionType, ref int width, ref int height, ref int bitDepth);
        int handle;
    }

    class WindowsImageSupport : IImageSupport
    {
        
        
        public WindowsImageSupport()
        {
        }
        public async Task<ImageProperties> GetImageProperties(ImageProperties imgProp)
        {
            OpenCVComputeAPIhandle.handle = IOpenCVComputeAPI.CreateOCVCInstance();
            ImageProperties imgPropOut = imgProp;
            imgPropOut.outAllocatedSize = ImageProperties.maxSize;

            safeBuf interBuf = new safeBuf();
            interBuf.buf = new byte[imgPropOut.outAllocatedSize];
            interBuf.gchandle = GCHandle.Alloc(interBuf.buf, GCHandleType.Pinned);
            interBuf.pBuf = interBuf.gchandle.AddrOfPinnedObject();

            imgPropOut.outBuf.buf = new byte[imgPropOut.outAllocatedSize];
            imgPropOut.outBuf.gchandle = GCHandle.Alloc(imgPropOut.outBuf.buf, GCHandleType.Pinned);
            imgPropOut.outBuf.pBuf = imgPropOut.outBuf.gchandle.AddrOfPinnedObject();

            IOpenCVComputeAPI.DecompressImage(OpenCVComputeAPIhandle.handle, imgPropOut.inBuf, imgPropOut.inSize, interBuf.pBuf, imgPropOut.outAllocatedSize, ref imgPropOut.outRealSize, ref imgPropOut.compressionType, ref imgPropOut.subCompressionType,
                 ref imgPropOut.width, ref imgPropOut.height, ref imgPropOut.bitDepth);
            IOpenCVComputeAPI.PostProc(OpenCVComputeAPIhandle.handle, imgPropOut.outBuf.pBuf, imgPropOut.outAllocatedSize, ref imgPropOut.outRealSize, ref imgPropOut.compressionType, ref imgPropOut.subCompressionType,
                 ref imgPropOut.width, ref imgPropOut.height, ref imgPropOut.bitDepth);
            return imgPropOut;

        }
        public async Task<AuthentiDocTake2.ImageFormat> GetFormatImageFileAsync(MemoryStream fileStream)
        {
            BitmapImage src = new BitmapImage();
            await src.SetSourceAsync(fileStream.AsRandomAccessStream());
            byte[] buffer = ReadFully(fileStream);
            AuthentiDocTake2.ImageFormat format = GetImageFormat(buffer);
            return format;
        }
    
        public static byte[] ReadFully(Stream input)
        {
            byte[] buffer = new byte[16 * 1024];
            using (MemoryStream ms = new MemoryStream())
            {
                int read;
                while ((read = input.Read(buffer, 0, buffer.Length)) > 0)
                {
                    ms.Write(buffer, 0, read);
                }
                return ms.ToArray();
            }
        }
       

        public static AuthentiDocTake2.ImageFormat GetImageFormat(byte[] bytes)
        {
            // see http://www.mikekunz.com/image_file_header.html  
            var bmp = Encoding.UTF8.GetBytes("BM");     // BMP
            var gif = Encoding.UTF8.GetBytes("GIF");    // GIF
            var png = new byte[] { 137, 80, 78, 71 };    // PNG
            var tiff = new byte[] { 73, 73, 42 };         // TIFF
            var tiff2 = new byte[] { 77, 77, 42 };         // TIFF
            var jpeg = new byte[] { 255, 216, 255, 224 }; // jpeg
            var jpeg2 = new byte[] { 255, 216, 255, 225 }; // jpeg canon

            if (bmp.SequenceEqual(bytes.Take(bmp.Length)))
                return AuthentiDocTake2.ImageFormat.Bmp;

            if (gif.SequenceEqual(bytes.Take(gif.Length)))
                return AuthentiDocTake2.ImageFormat.Gif;

            if (png.SequenceEqual(bytes.Take(png.Length)))
                return AuthentiDocTake2.ImageFormat.Png;

            if (tiff.SequenceEqual(bytes.Take(tiff.Length)))
                return AuthentiDocTake2.ImageFormat.Tiff;

            if (tiff2.SequenceEqual(bytes.Take(tiff2.Length)))
                return AuthentiDocTake2.ImageFormat.Tiff;

            if (jpeg.SequenceEqual(bytes.Take(jpeg.Length)))
                return AuthentiDocTake2.ImageFormat.Jpeg;

            if (jpeg2.SequenceEqual(bytes.Take(jpeg2.Length)))
                return AuthentiDocTake2.ImageFormat.Jpeg;

            return AuthentiDocTake2.ImageFormat.Unknown;
        }
    }
}
