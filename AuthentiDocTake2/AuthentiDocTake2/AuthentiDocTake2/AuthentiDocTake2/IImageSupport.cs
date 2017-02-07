using System;
using System.IO;
using System.Threading.Tasks;

namespace AuthentiDocTake2
{
    public interface IFileSupport
    {
        Task<MemoryStream> FileToStream(string fileName);
    }
    public interface IImageSupport
    {
        Task<ImageFormat> GetFormatImageFileAsync(MemoryStream fileStream);
        Task<ImageProperties> GetImageProperties(ImageProperties imgProp);
    }
    public enum ImageFormat : int
        {
            Bmp,
            Jpeg,
            Gif,
            Tiff,
            Png,
            Unknown
        }
    public struct safeBuf
    {
        public byte[] buf;
        public System.Runtime.InteropServices.GCHandle gchandle;
        public IntPtr pBuf;
    }
    public struct ImageProperties
    {
        public const int maxSize = 3840 * 2160 * 4; //4K RGB 32 bit resolution
        public IntPtr inBuf;
        public int inSize;
        public safeBuf outBuf;
        public int outAllocatedSize;
        public int outRealSize;
        public int compressionType;
        public int subCompressionType;
        public int width;
        public int height;
        public int bitDepth;
    }
}
