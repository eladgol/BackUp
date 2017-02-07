using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;

namespace AuthentiDocTake2
{
    public class LoadImageFormats
    {
        public InputImage BMPToImage(Stream BMPstream)
        {


            byte[] imageBMPBuf = BMPstream.ToByteArray();
            GCHandle pinnedImageBuf = GCHandle.Alloc(imageBMPBuf, GCHandleType.Pinned);
            IntPtr pBMPImage = pinnedImageBuf.AddrOfPinnedObject();

            var bitmapInfo = Marshal.PtrToStructure<BITMAPINFOHEADER>(pBMPImage);
            IntPtr pActualImage = Marshal.UnsafeAddrOfPinnedArrayElement(imageBMPBuf, (int)bitmapInfo.biImagOffset);
            byte[] imageOutBuf = new byte[bitmapInfo.biHeight * bitmapInfo.biWidth * bitmapInfo.biBitCount / 8];
            GCHandle pinnedOutImageBuf = GCHandle.Alloc(imageOutBuf, GCHandleType.Pinned);
            IntPtr pOutActualImage = Marshal.UnsafeAddrOfPinnedArrayElement(imageOutBuf, (int)0);
            ReverseBits(pActualImage, pOutActualImage, bitmapInfo.biWidth, bitmapInfo.biHeight, bitmapInfo.biBitCount / 8);
            int byteWidth = bitmapInfo.biWidth * bitmapInfo.biBitCount / 8;
            int pitchBytes = (byteWidth % 4) + byteWidth;
            return new InputImage(pOutActualImage, byteWidth, bitmapInfo.biWidth, bitmapInfo.biHeight, bitmapInfo.biBitCount / 8);
        }
      
        private unsafe void ReverseBits(IntPtr InPtr, IntPtr OutPtr, int width, int height, int bytesPerPixel)
        {
            byte* src = (byte*)InPtr.ToPointer();
            byte* dst = (byte*)OutPtr.ToPointer();
            int byteWidth = width * bytesPerPixel;
            int paddedWidth = (byteWidth % 4) + byteWidth;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < byteWidth; x++)
                {
                    dst[x + byteWidth * y] = src[x + paddedWidth * (height - y - 1)];
                }
            }
        }
    }
    public static class StreamExtensions
    {
        public enum Endianness
        {
            BigEndian,
            LittleEndian
        }

        [AttributeUsage(AttributeTargets.Field)]
        public class EndianAttribute : Attribute
        {
            public Endianness Endianness { get; private set; }

            public EndianAttribute(Endianness endianness)
            {
                this.Endianness = endianness;
            }
        }
        private static void RespectEndianness(Type type, byte[] data)
        {
            foreach (FieldInfo f in type.GetRuntimeFields())
            {
                if (f.IsDefined(typeof(EndianAttribute), false))
                {
                    EndianAttribute att = (EndianAttribute)f.GetCustomAttributes(typeof(EndianAttribute), false).GetEnumerator().Current;
                    int offset = Marshal.OffsetOf(type, f.Name).ToInt32();
                    if ((att.Endianness == Endianness.BigEndian && BitConverter.IsLittleEndian) ||
                        (att.Endianness == Endianness.LittleEndian && !BitConverter.IsLittleEndian))
                    {
                        Array.Reverse(data, offset, Marshal.SizeOf(f.FieldType));
                    }
                }
            }
        }
        public static byte[] ToByteArray(this Stream stream)
        {
            using (var ms = new MemoryStream())
            {
                stream.CopyTo(ms);
                return (ms.ToArray());
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BITMAPINFOHEADER
    {
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public ushort biFixed; //0x4240
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biSize;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public ushort biDontCare1;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public ushort biDontCare2;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biImagOffset;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biSize2;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public int biWidth;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public int biHeight;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public ushort biPlanes;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public ushort biBitCount;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biCompression;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biSizeImage;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public int biXPelsPerMeter;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public int biYPelsPerMeter;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biClrUsed;
        [StreamExtensions.EndianAttribute(StreamExtensions.Endianness.BigEndian)]
        public uint biClrImportant;

        public void Init()
        {
            biSize = (uint)Marshal.SizeOf(this);
        }
    }
}
