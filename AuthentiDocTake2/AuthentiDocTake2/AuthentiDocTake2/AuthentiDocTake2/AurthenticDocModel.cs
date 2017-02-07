using System;
using System.Collections.Generic;
using System.Text;

using System.Threading.Tasks;

using Xamarin.Forms;
using System.Runtime.CompilerServices;
using System.ComponentModel;
using XLabs.Ioc;
using XLabs.Platform;
using XLabs.Platform.Device;
using XLabs.Forms;
using Xamarin.Forms.Platform;
using System.Diagnostics;
using System.Windows.Input;
using AuthentiDocTake2.Support;
using XLabs.Platform.Services.Media;
using System.IO;
using System.IO.Compression;
using System.Runtime.InteropServices;
//using SkiaSharp;

namespace AuthentiDocTake2
{
    public class InputImage
    {
        public IntPtr imageBuf { get; set; }
        public int width { get; set; }
        public int height { get; set; }
        public int pitchBytes { get; set; }
        public int bytesPerPixel { get; set; }
        public InputImage(IntPtr _imageBuf, int _pitchBytes, int _width, int _height, int _bytesPerPixel)
        {
            imageBuf = _imageBuf;
            width = _width;
            pitchBytes = _pitchBytes;
            height = _height;
            bytesPerPixel = _bytesPerPixel;
        }
    }
    public class ObservableObject : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged = delegate { };
        public void OnPropertyChanged(string propertyName)
        {
            PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
    // [ImplementPropertyChanged]
    public class AuthenticDocModel : ObservableObject
    {
        public string buttonCameraName { get; set; }

        private ImageSource imageSourcel;
        public ImageSource imageSource {
            get { return imageSourcel; }
            set
            {
                imageSourcel = value;
                OnPropertyChanged(nameof(imageSource));
            }
        }
        public string urlEntryText { get; set; }
        public int urlEntryItemsSelected { get; set; }
        public ResourceDictionary Resources { get; set; }

        private string textResult;
        public string TextResult
        {
            get { return textResult; }
            set
            {
                textResult = value;
                OnPropertyChanged(nameof(TextResult));
            }
        }

        private bool canExecute = true;
        private bool canExecuteProcess = true;
        public bool CanExecuteProcess 
        {
            get { return canExecuteProcess; }
            set { canExecuteProcess = value; ProcessCommand.CanExecute(canExecuteProcess); }
        }
        private ICommand takePictureFromCameraCommand;
        public ICommand TakePictureFromCameraCommand
        {
            get { return takePictureFromCameraCommand; }
            set { takePictureFromCameraCommand = value; }
        }
        private ICommand loadImageFromFileCommand;
        public ICommand LoadImageFromFileCommand
        {
             get { return loadImageFromFileCommand; }
             set { loadImageFromFileCommand = value; }
        }
        private ICommand loadImageFromURLCommand;
        public ICommand LoadImageFromURLCommand
        {
            get { return loadImageFromURLCommand; }
            set { loadImageFromURLCommand = value; }
        }
        private ICommand testButtonCommand;
        public ICommand TestButtonCommand
        {
            get { return testButtonCommand; }
            set { testButtonCommand = value; }
        }
        private ICommand urlEntryPickerChangedCommand;
        public ICommand URLEntryPickerChangedCommand
        {
            get { return urlEntryPickerChangedCommand; }
            set { urlEntryPickerChangedCommand = value; }
        }
        private ICommand processCommand;
        public ICommand ProcessCommand
        {
            get { return processCommand; }
            set { processCommand = value; }
        }
        private IDevice m_currentDevice;
        private IMediaPicker m_mediaPicker;
        private SignalProcess m_signalProcess;
        private IThreadingCrossPlatform m_IThreading;
        private IFileSupport m_IFileSupport;
        private IImageSupport m_IImageSupport;
        private ProcessingImage processImage;
        private EventHandler<ImageSource> updateImageEvent;
        private string[] m_testURls;
        public AuthenticDocModel(SignalProcess _process, IThreadingCrossPlatform _IThreading, IDevice _currentDevice, IMediaPicker _mediaPicker, IImageSupport _imageSupport, IFileSupport _fileSupport, string[] _testURls)
        {
            m_IThreading = _IThreading;
            m_IFileSupport = _fileSupport;
            m_IImageSupport = _imageSupport;
            m_testURls = _testURls;

            SetupMedia(_currentDevice, _mediaPicker);

            buttonCameraName = "Camera";

            //urlEntryText = directory;
            TakePictureFromCameraCommand = new RelayCommand(TakePictureFromCameraRelay, param => this.canExecute);
            LoadImageFromFileCommand = new RelayCommand(LoadImageFromFileCommandRelay, param => this.canExecute);
            LoadImageFromURLCommand = new RelayCommand(LoadImageFromURLCommandRelay, param => this.canExecute);
            TestButtonCommand = new RelayCommand(TestButtonCommandRelay, param => this.canExecute);
            ProcessCommand = new RelayCommand(ProcessCommandRelay, param => this.CanExecuteProcess);
            URLEntryPickerChangedCommand = new RelayCommand(urlEntryPickerChangedCommandRelay, param => this.canExecute);
            updateImageEvent += new EventHandler<ImageSource>((o, s) => { imageSource = s; });
            imageSource = new UriImageSource
            {
                Uri = new Uri("https://grandprix20.files.wordpress.com/2012/11/317757_423537204379996_1874288384_n-1.png"),
                CachingEnabled = false
            };
            processImage = new ProcessingImage(_process);

        }
        public async void TakePictureFromCameraRelay(object obj)
        {
           await TakePictureFromCamera();
        }
        public async void ProcessCommandRelay(object obj)
        {
            TextResult =  await processImage.GetText();
            //List<SkiaSharp.SKRectI> boxes = new List<SkiaSharp.SKRectI>();
            //boxes = await processImage.GetBoxes();
            //int iBox = 0;
            //foreach (var box in boxes)
            //{
            //    Debug.WriteLine("box #" + iBox.ToString() + " (" + box.Left + "," + box.Top + "," + box.Right + "," + box.Bottom + ")");
            //    iBox++;
            //}
        }
        public async void LoadImageFromFileCommandRelay(object obj)
        {
           imageSource = await LoadImageFromFile(urlEntryText);

            
        }
        public async void LoadImageFromURLCommandRelay(object obj)
        {
            /*Task.Run(async () => {*/ await LoadImageFromURL(urlEntryText, updateImageEvent); /*});*/


        }
        public async void TestButtonCommandRelay(object obj)
        {
            imageSource = new UriImageSource
            {
                Uri = new Uri("http://www.nasa.gov/sites/default/files/styles/1600x1200_autoletterbox/public/images/298773main_EC02-0282-3_full.jpg"),
                CachingEnabled = false
            };
            buttonCameraName = "Test";
        }
        public async void urlEntryPickerChangedCommandRelay(object obj)
        {
            urlEntryText = m_testURls[urlEntryItemsSelected];
            
        }
        private async Task<ImageSource> TakePictureFromCamera()
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "TakePictureFromCamera");

            MediaFile picTaken = await TakePicture();

            imageSource = ImageSource.FromStream(() =>
            {
                if (picTaken != null)
                    return picTaken.Source;
                else
                    return null;
            });
            CanExecuteProcess = true;
            return null;
        }


        private async Task<MediaFile> TakePicture()
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "TakePicture");
            return await m_mediaPicker.TakePhotoAsync(new CameraMediaStorageOptions { DefaultCamera = CameraDevice.Front, MaxPixelDimension = 400 }).ContinueWith(t =>
            {
                threadID = m_IThreading.GetCurrentThreadID().ToString();
                System.Diagnostics.Debug.WriteLine(threadID + "after TakePhotoAsync");
                string Status;
                if (t.IsFaulted)
                {
                    Status = t.Exception.InnerException.ToString();
                }
                else if (t.IsCanceled)
                {
                    Status = "Canceled";
                }
                else
                {
                    MediaFile mFile = t.Result;
                    return mFile;
                }

                return null;
            }, TaskScheduler.FromCurrentSynchronizationContext());
        }
        private async Task<ImageSource> LoadImageFromFile(String sUri)
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "LoadImageFromURL");
            var fileStream = await m_IFileSupport.FileToStream(sUri);
            ImageFormat format = await m_IImageSupport.GetFormatImageFileAsync(fileStream);
            
            ImageSource source = ImageSource.FromStream(() => fileStream);
            Stream imageStream = new System.IO.MemoryStream();
            var asyncUrlDownload = new AsyncUrlDownload(m_IThreading);
            var streamBMP = await asyncUrlDownload.DownloadHttpToArray(sUri);


            if (streamBMP.Item1)
            {
                var bmpConverter = new LoadImageFormats();
                processImage.inputImage = bmpConverter.BMPToImage(streamBMP.Item2);
            }

            CanExecuteProcess = true;
            return source;
        }
        private async Task<ImageProperties> GetImageProperties(Stream stream)
        {
            ImageProperties imgProp = new ImageProperties();

            byte[] imageBuf = stream.ToByteArray();
            GCHandle pinnedImageBuf = GCHandle.Alloc(imageBuf, GCHandleType.Pinned);
            IntPtr pImage = pinnedImageBuf.AddrOfPinnedObject();
            imgProp.inBuf = pImage;
            imgProp.inSize = imageBuf.Length;
            return await m_IImageSupport.GetImageProperties(imgProp);
               
        }
        private async Task<ImageSource> LoadImageFromURL(String sUri,  EventHandler<ImageSource> UpdateImageEvent)
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "LoadImageFromURL");
            var source = new UriImageSource
            {
                Uri = new Uri(sUri),
                CachingEnabled = false
            };

            //Stream imageStream = new System.IO.MemoryStream();
            var asyncUrlDownload = new AsyncUrlDownload(m_IThreading);
            var stream = await asyncUrlDownload.DownloadHttpToArray(sUri);

            //var bmpConverter = new LoadImageFormats();
            //processImage.inputImage = bmpConverter.BMPToImage(stream);
            if (stream.Item1)
            {
                ImageProperties imgProp = await GetImageProperties(stream.Item2);
                //System.Diagnostics.Debug.WriteLine(threadID + "LoadImageFromURL - res {0}x{1} depth {2} compression {3} subComp {4} inSize {5} outSize {6}", 
                //    imgProp.width, imgProp.height, imgProp.bitDepth, imgProp.compressionType, imgProp.subCompressionType, imgProp.inSize, imgProp.outRealSize);
                processImage.inputImage = new InputImage(imgProp.outBuf.pBuf, (int)(imgProp.width * imgProp.bitDepth / 8.0), imgProp.width, imgProp.height, (int)(imgProp.bitDepth / 8.0));
                CanExecuteProcess = true;
                UpdateImageEvent(this, source);
            }
            return source;
        }
        private void SetupMedia(IDevice _currentDevice, IMediaPicker _mediaPicker)
        {
            m_currentDevice = ResolveDevice(_currentDevice);
            m_mediaPicker = SetupMedia(_mediaPicker);
        }
        private IDevice ResolveDevice(IDevice currentDevice)
        {
            var resolverContainer = new SimpleContainer();

            resolverContainer.Register<IDevice>(t => currentDevice);
            Resolver.SetResolver(resolverContainer.GetResolver());
            return (Resolver.Resolve<IDevice>());
        }
        IMediaPicker SetupMedia(IMediaPicker _mediaPicker)
        {
            if (_mediaPicker != null)
            {
                return _mediaPicker;
            }
            return (DependencyService.Get<IMediaPicker>() ?? m_currentDevice.MediaPicker);
        }

        private class ProcessingImage
        {
            
            public InputImage inputImage { get; set; }
            public string ocrResult { get; set; }
            //public List<SkiaSharp.SKRectI> boxes { get; set; }
            private SignalProcess signalProcess;
            
            public ProcessingImage(SignalProcess _signalProcess)
            {
                inputImage = new InputImage(IntPtr.Zero, 0, 0, 0, 0);
                ocrResult = string.Empty;
                signalProcess = _signalProcess;
            }
            public async Task<string> GetText()
            {
                if (inputImage == null || inputImage.imageBuf == IntPtr.Zero
                    || inputImage.width <= 0 || inputImage.height <= 0 || inputImage.bytesPerPixel <= 0)
                {
                    return string.Empty;
                }
                else
                {
                    ocrResult = await signalProcess.GetText(inputImage);
                    return ocrResult;
                }
            }
            //public async Task<List<SkiaSharp.SKRectI>> GetBoxes()
            //{
            //    if (inputImage == null || inputImage.imageBuf == IntPtr.Zero
            //        || inputImage.width <= 0 || inputImage.height <= 0 || inputImage.bytesPerPixel <= 0
            //        || ocrResult == string.Empty)
            //    {
            //        return null;
            //    }
            //    else
            //    {
            //        boxes = await signalProcess.GetBoxes(inputImage);
            //        return boxes;
            //    }
            //}
        }
    }
}

