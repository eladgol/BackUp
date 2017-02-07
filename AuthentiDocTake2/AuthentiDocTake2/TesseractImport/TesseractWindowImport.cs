using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TesseractImport
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
        public const string LeptonicaDllName = "C:\\Code\\MyCode\\AuthentiDocTake2\\AuthentiDocTake2\\AuthentiDocTake2\\AuthentiDocTake2.Windows\\x86\\liblept172";
        public const string TesseractDllName = "C:\\Code\\MyCode\\AuthentiDocTake2\\AuthentiDocTake2\\AuthentiDocTake2\\AuthentiDocTake2.Windows\\x86\\libtesseract304";
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
        public static extern IntPtr BaseAPIAnalyseLayout(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIClear")]
        public static extern void BaseAPIClear(int handle);

        /// <summary>
        /// Creates a new BaseAPI instance
        /// </summary>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPICreate")]
        public static extern IntPtr BaseApiCreate();

        // Base API
        /// <summary>
        /// Deletes a base api instance.
        /// </summary>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIDelete")]
        public static extern void BaseApiDelete(int ptr);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIDetectOS")]
        public static extern int BaseAPIDetectOS(int handle, ref OSResult result);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetBoolVariable")]
        public static extern int BaseApiGetBoolVariable(int handle, string name, out int value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetDoubleVariable")]
        public static extern int BaseApiGetDoubleVariable(int handle, string name, out double value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetHOCRText")]
        public static extern IntPtr BaseAPIGetHOCRTextInternal(int handle, int pageNum);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetIntVariable")]
        public static extern int BaseApiGetIntVariable(int handle, string name, out int value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetIterator")]
        public static extern IntPtr BaseApiGetIterator(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetPageSegMode")]
        public static extern PageSegMode BaseAPIGetPageSegMode(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetStringVariable")]
        public static extern IntPtr BaseApiGetStringVariableInternal(int handle, string name);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetThresholdedImage")]
        public static extern IntPtr BaseAPIGetThresholdedImage(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIGetUTF8Text")]
        public static extern IntPtr BaseAPIGetUTF8TextInternal(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIInit4")]
        public static extern int BaseApiInit(int handle, string datapath, string language, int mode,
                                      string[] configs, int configs_size,
                                      string[] vars_vec, string[] vars_values, UIntPtr vars_vec_size,
                                      bool set_only_non_debug_params);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIMeanTextConf")]
        public static extern int BaseAPIMeanTextConf(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPIRecognize")]
        public static extern int BaseApiRecognize(int handle, int monitor);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetDebugVariable")]
        public static extern int BaseApiSetDebugVariable(int handle, string name, IntPtr valPtr);

        // image analysis
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetImage2")]
        public static extern void BaseApiSetImage(int handle, int pixHandle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetInputName")]
        public static extern void BaseApiSetInputName(int handle, string value);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetPageSegMode")]
        public static extern void BaseAPISetPageSegMode(int handle, PageSegMode mode);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetRectangle")]
        public static extern void BaseApiSetRectangle(int handle, int left, int top, int width, int height);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessBaseAPISetVariable")]
        public static extern int BaseApiSetVariable(int handle, string name, IntPtr valPtr);

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
        public static extern int PageIteratorBaseline(int handle, PageIteratorLevel level, out int x1, out int y1, out int x2, out int y2);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBegin")]
        public static extern void PageIteratorBegin(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBlockType")]
        public static extern PolyBlockType PageIteratorBlockType(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorBoundingBox")]
        public static extern int PageIteratorBoundingBox(int handle, PageIteratorLevel level, out int left, out int top, out int right, out int bottom);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorCopy")]
        public static extern IntPtr PageIteratorCopy(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorDelete")]
        public static extern void PageIteratorDelete(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorGetBinaryImage")]
        public static extern IntPtr PageIteratorGetBinaryImage(int handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorGetImage")]
        public static extern IntPtr PageIteratorGetImage(int handle, PageIteratorLevel level, int padding, int originalImage, out int left, out int top);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorIsAtBeginningOf")]
        public static extern int PageIteratorIsAtBeginningOf(int handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorIsAtFinalElement")]
        public static extern int PageIteratorIsAtFinalElement(int handle, PageIteratorLevel level, PageIteratorLevel element);

        // page iterator
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorNext")]
        public static extern int PageIteratorNext(int handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessPageIteratorOrientation")]
        public static extern void PageIteratorOrientation(int handle, out Orientation orientation, out WritingDirection writing_direction, out TextLineOrder textLineOrder, out float deskew_angle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorCopy")]
        public static extern IntPtr ResultIteratorCopy(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorDelete")]
        public static extern void ResultIteratorDelete(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorConfidence")]
        public static extern float ResultIteratorGetConfidence(int handle, PageIteratorLevel level);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetPageIterator")]
        public static extern IntPtr ResultIteratorGetPageIterator(int handle);

        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetUTF8Text")]
        public static extern IntPtr ResultIteratorGetUTF8TextInternal(int handle, PageIteratorLevel level);

        #region Choice Iterator

        /// <summary>
        /// Native API call to TessResultIteratorGetChoiceIterator
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessResultIteratorGetChoiceIterator")]
        public static extern IntPtr ResultIteratorGetChoiceIterator(int handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorDelete
        /// </summary>
        /// <param name="handle"></param>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorDelete")]
        public static extern void ChoiceIteratorDelete(int handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorNext
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorNext")]
        public static extern int ChoiceIteratorNext(int handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorGetUTF8Text
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorGetUTF8Text")]
        public static extern IntPtr ChoiceIteratorGetUTF8TextInternal(int handle);

        /// <summary>
        /// Native API call to TessChoiceIteratorConfidence
        /// </summary>
        /// <param name="handle"></param>
        /// <returns></returns>
        [DllImport(Constants.TesseractDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "TessChoiceIteratorConfidence")]
        public static extern float ChoiceIteratorGetConfidence(int handle);

        #endregion
    }

 
}
