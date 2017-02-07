using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AuthentiDocTake2.WindowsShared
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    using System.Text;

    
    [StructLayout(LayoutKind.Sequential)]
    public struct OSBestResult
    {
        public int OrientationId;
        public int ScriptId;
        public float SConfidence;
        public float OConfidence;
    }
    public enum Orientation
    {
        /// <summary>
        /// Page is correctly alligned with up and no rotation is needed.
        /// </summary>
        PageUp,
        /// <summary>
        /// Page needs to be rotated so the right hand side is up, 90 degress counter clockwise, to be readable.
        /// </summary>
        PageRight,
        /// <summary>
        /// Page needs to be rotated so the bottom side is up, 180 degress counter clockwise, to be readable.
        /// </summary>
        PageDown,
        /// <summary>
        /// Page needs to be rotated so the left hand side is up, 90 degress clockwise, to be readable.
        /// </summary>
        PageLeft
    }
    public enum PageIteratorLevel : int
    {
        Block,
        Para,
        TextLine,
        Word,
        Symbol
    }
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct OSResult
    {
        public void Init()
        {
            fixed (float* orientationsPtr = _orientations)
            {
                fixed (float* scriptsNAPtr = _scriptsNA)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        for (int j = 0; j < kMaxNumberOfScripts; ++j)
                        {
                            scriptsNAPtr[i * kMaxNumberOfScripts + j] = 0f;
                        }
                        orientationsPtr[i] = 0f;
                    }
                }
            }
            _unicharset = IntPtr.Zero;
            _bestResult = new OSBestResult();
        }

        // Max number of scripts in ICU + "NULL" + Japanese and Korean + Fraktur
        private const int kMaxNumberOfScripts = 116 + 1 + 2 + 1;

        private fixed float _orientations[4];

        // Script confidence scores for each of 4 possible orientations.
        private fixed float _scriptsNA[4 * kMaxNumberOfScripts];

        private IntPtr _unicharset;
        private OSBestResult _bestResult;

        public void GetBestOrientation(out Orientation orientation, out float confidence)
        {
            switch (_bestResult.OrientationId)
            {
                case 0: orientation = Orientation.PageUp; break;
                case 1: orientation = Orientation.PageRight; break;
                case 2: orientation = Orientation.PageDown; break;
                case 3: orientation = Orientation.PageLeft; break;
                default: throw new InvalidOperationException("Best orientation must be between 0 and 3 but was " + _bestResult.OrientationId + ".");
            }

            confidence = _bestResult.OConfidence;
        }
    }
   
    internal static class Constants
    {
#if  WINDOWS_APP
        public const string LeptonicaDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x64\Debug\AppX\liblept.dll";
        //public const string TesseractDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x86\Debug\AppX\libtesseract304.dll";
        public const string TesseractDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x64\Debug\AppX\libtesseract.dll";
        public const string TesseractDataSetPath = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x64\Debug\AppX";
#endif
#if  WINDOWS_UWP
        public const string LeptonicaDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.UWP\bin\x64\Debug\AppX\liblept.dll";
        //public const string TesseractDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.Windows\bin\x86\Debug\AppX\libtesseract304.dll";
        public const string TesseractDllName = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.UWP\bin\x64\Debug\AppX\libtesseract.dll";
        public const string TesseractDataSetPath = @"C:\Code\MyCode\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2\AuthentiDocTake2.UWP\bin\x64\Debug\AppX";
#endif
    }
    public enum PageSegMode : int
    {
        /// <summary>
        /// Orientation and script detection (OSD) only.
        /// </summary>
        OsdOnly,

        /// <summary>
        /// Automatic page sementation with orientantion and script detection (OSD).
        /// </summary>
        AutoOsd,

        /// <summary>
        /// Automatic page segmentation, but no OSD, or OCR.
        /// </summary>
        AutoOnly,

        /// <summary>
        /// Fully automatic page segmentation, but no OSD.
        /// </summary>
        Auto,

        /// <summary>
        /// Assume a single column of text of variable sizes.
        /// </summary>
        SingleColumn,

        /// <summary>
        /// Assume a single uniform block of vertically aligned text.
        /// </summary>
        SingleBlockVertText,

        /// <summary>
        /// Assume a single uniform block of text.
        /// </summary>
        SingleBlock,

        /// <summary>
        /// Treat the image as a single text line.
        /// </summary>
        SingleLine,

        /// <summary>
        /// Treat the image as a single word.
        /// </summary>
        SingleWord,

        /// <summary>
        /// Treat the image as a single word in a circle.
        /// </summary>
        CircleWord,

        /// <summary>
        /// Treat the image as a single character.
        /// </summary>
        SingleChar,

        /// <summary>
        /// Number of enum entries.
        /// </summary>
        Count
    }
    public enum PolyBlockType : int
    {
        Unknown,
        FlowingText,
        HeadingText,
        PullOutText,
        Table,
        VerticalText,
        CaptionText,
        FlowingImage,
        HeadingImage,
        PullOutImage,
        HorizontalLine,
        VerticalLine,
        Noise,
        Count
    }
    /// <summary>
    /// The grapheme cluster within a line of text are laid out logically in this direction,
    /// judged when looking at the text line rotated so that Orientation is "page up".
    /// </summary>
    public enum WritingDirection
    {
        /// <summary>
        /// The text line from the left hand side to the right hand side when the page is rotated so it's orientation is <see cref="Orientation.PageUp" />.
        /// </summary>
        LeftToRight,

        /// <summary>
        /// The text line from the right hand side to the left hand side when the page is rotated so it's orientation is <see cref="Orientation.PageUp" />.
        /// </summary>
        RightToLeft,

        /// <summary>
        /// The text line from the top to the bottom of the page when the page is rotated so it's orientation is <see cref="Orientation.PageUp" />.
        /// </summary>
        TopToBottom
    }
    public enum TextLineOrder : int
    {
        /// <summary>
        /// The text lines form vertical columns ordered left to right.
        /// </summary>
        LeftToRight,

        /// <summary>
        /// The text lines form vertical columns ordered right to left.
        /// </summary>
        RightToLeft,

        /// <summary>
        /// The text lines form horizontal columns ordered top to bottom.
        /// </summary>
        TopToBottom
    }
    public class ITessApiSignatures
    {
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIAnalyseLayout")]
        public static extern IntPtr BaseAPIAnalyseLayout(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIClear")]
        public static extern void BaseAPIClear(IntPtr handle);

        /// <summary>
        /// Creates a new BaseAPI instance
        /// </summary>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPICreate")]
        public static extern IntPtr BaseApiCreate();
        public delegate IntPtr delegateBaseApiCreate();
        // Base API
        /// <summary>
        /// Deletes a base api instance.
        /// </summary>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIDelete")]
        public static extern void BaseApiDelete(int ptr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIDetectOS")]
        public static extern int BaseAPIDetectOS(IntPtr handle, ref OSResult result);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetBoolVariable")]
        public static extern int BaseApiGetBoolVariable(IntPtr handle, string name, out int value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetDoubleVariable")]
        public static extern int BaseApiGetDoubleVariable(IntPtr handle, string name, out double value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetHOCRText")]
        public static extern IntPtr BaseAPIGetHOCRTextInternal(IntPtr handle, int pageNum);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetIntVariable")]
        public static extern int BaseApiGetIntVariable(IntPtr handle, string name, out int value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetIterator")]
        public static extern IntPtr BaseApiGetIterator(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetPageSegMode")]
        public static extern PageSegMode BaseAPIGetPageSegMode(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetStringVariable")]
        public static extern IntPtr BaseApiGetStringVariableInternal(IntPtr handle, string name);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetThresholdedImage")]
        public static extern IntPtr BaseAPIGetThresholdedImage(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetUTF8Text")]
        public static extern IntPtr BaseAPIGetUTF8TextInternal(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIInit4")]
        public static extern int BaseApiInit(IntPtr handle, string datapath, string language, int mode,
                                      string[] configs, int configs_size,
                                      string[] vars_vec, string[] vars_values, UIntPtr vars_vec_size,
                                      bool set_only_non_debug_params);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIInit3")]
        public static extern int BaseApiInit3(IntPtr handle, string datapath, string language);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIMeanTextConf")]
        public static extern int BaseAPIMeanTextConf(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIRecognize")]
        public static extern int BaseApiRecognize(IntPtr handle, IntPtr monitor);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetDebugVariable")]
        public static extern int BaseApiSetDebugVariable(IntPtr handle, string name, IntPtr valPtr);

        // image analysis
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetImage2")]
        public static extern void BaseApiSetImage(IntPtr handle, IntPtr pixHandle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetImage")]
        public static extern void BaseApiSetImageDirect(IntPtr handle, IntPtr imageData, int width, int height,
                                                  int bytes_per_pixel, int bytes_per_line);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetInputName")]
        public static extern void BaseApiSetInputName(IntPtr handle, string value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetPageSegMode")]
        public static extern void BaseAPISetPageSegMode(IntPtr handle, PageSegMode mode);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetRectangle")]
        public static extern void BaseApiSetRectangle(IntPtr handle, int left, int top, int width, int height);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetVariable")]
        public static extern int BaseApiSetVariable(IntPtr handle, string name, IntPtr valPtr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessDeleteBlockList")]
        public static extern void DeleteBlockList(IntPtr arr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessDeleteIntArray")]
        public static extern void DeleteIntArray(IntPtr arr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessDeleteText")]
        public static extern void DeleteText(IntPtr textPtr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessDeleteTextArray")]
        public static extern void DeleteTextArray(IntPtr arr);

        // Helper functions
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessVersion")]
        public static extern string GetVersion();

        // result iterator

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBaseline")]
        public static extern int PageIteratorBaseline(IntPtr handle, PageIteratorLevel level, out int x1, out int y1, out int x2, out int y2);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBegin")]
        public static extern void PageIteratorBegin(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBlockType")]
        public static extern PolyBlockType PageIteratorBlockType(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBoundingBox")]
        public static extern int PageIteratorBoundingBox(IntPtr handle, PageIteratorLevel level, out int left, out int top, out int right, out int bottom);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorCopy")]
        public static extern IntPtr PageIteratorCopy(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorDelete")]
        public static extern void PageIteratorDelete(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorGetBinaryImage")]
        public static extern IntPtr PageIteratorGetBinaryImage(IntPtr handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorGetImage")]
        public static extern IntPtr PageIteratorGetImage(IntPtr handle, PageIteratorLevel level, int padding, int originalImage, out int left, out int top);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorIsAtBeginningOf")]
        public static extern int PageIteratorIsAtBeginningOf(IntPtr handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorIsAtFinalElement")]
        public static extern int PageIteratorIsAtFinalElement(IntPtr handle, PageIteratorLevel level, PageIteratorLevel element);

        // page iterator
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorNext")]
        public static extern int PageIteratorNext(IntPtr handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorOrientation")]
        public static extern void PageIteratorOrientation(IntPtr handle, out Orientation orientation, out WritingDirection writing_direction, out TextLineOrder textLineOrder, out float deskew_angle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorCopy")]
        public static extern IntPtr ResultIteratorCopy(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorDelete")]
        public static extern void ResultIteratorDelete(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorConfidence")]
        public static extern float ResultIteratorGetConfidence(IntPtr handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetPageIterator")]
        public static extern IntPtr ResultIteratorGetPageIterator(IntPtr handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetUTF8Text")]
        public static extern IntPtr ResultIteratorGetUTF8TextInternal(IntPtr handle, PageIteratorLevel level);

        #region Choice Iterator

        /// <summary>
        /// Native API call to TessResultIteratorGetChoiceIterator
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetChoiceIterator")]
        public static extern IntPtr ResultIteratorGetChoiceIterator(IntPtr handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorDelete
        /// </summary>
        /// <param name="handle"></param>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorDelete")]
        public static extern void ChoiceIteratorDelete(IntPtr handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorNext
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorNext")]
        public static extern int ChoiceIteratorNext(IntPtr handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorGetUTF8Text
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorGetUTF8Text")]
        public static extern IntPtr ChoiceIteratorGetUTF8TextInternal(IntPtr handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorConfidence
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorConfidence")]
        public static extern float ChoiceIteratorGetConfidence(IntPtr handle);

        #endregion


        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIRect")]
        public static extern IntPtr APIRect(IntPtr handle, IntPtr imagedata,
                                               int bytes_per_pixel, int bytes_per_line,
                                               int left, int top, int width, int height);


    }


}


