using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using XLabs.Platform.Services.Media;

using Windows.Media.Capture;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Graphics.Imaging;
using Windows.Foundation;
using System.IO;

namespace AuthentiDocTake2.WindowsShared
{
    class WindowsMediaPicker : IMediaPicker
    {
        CameraCaptureUI m_captureUI;
        public WindowsMediaPicker(Size szDisplay)
        {
            m_captureUI = new CameraCaptureUI();
            m_captureUI.PhotoSettings.Format = CameraCaptureUIPhotoFormat.Jpeg;      
            m_captureUI.PhotoSettings.CroppedSizeInPixels = szDisplay;
        }
        public bool IsCameraAvailable
        {
            get
            {
                
                throw new NotImplementedException();
            }
        }

        public bool IsPhotosSupported
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public bool IsVideosSupported
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public EventHandler<MediaPickerErrorArgs> OnError
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public EventHandler<MediaPickerArgs> OnMediaSelected
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public Task<MediaFile> SelectPhotoAsync(CameraMediaStorageOptions options)
        {
            throw new NotImplementedException();
        }

        public Task<MediaFile> SelectVideoAsync(VideoMediaStorageOptions options)
        {
            throw new NotImplementedException();
        }
        StorageFile m_photo;
        //MediaFile m_MediaFile;
        Stream m_Stream;
        private Stream getStream() { return m_Stream; }
        public async Task<MediaFile> TakePhotoAsync(CameraMediaStorageOptions options)
        {
            m_photo = await m_captureUI.CaptureFileAsync(CameraCaptureUIMode.Photo);
            m_Stream = await m_photo.OpenStreamForReadAsync();
            //   Func<string, string> convert = delegate (string s)
            // { return s.ToUpper(); };
            Func<Stream> conv = new Func<Stream>(getStream);///({ return await m_photo.OpenStreamForReadAsync(); };
            MediaFile p = new MediaFile(m_photo.Path, conv);
            if (m_photo == null)
            {
                // User cancelled photo capture
                return null;
            }
            return p; 
        }

        public Task<MediaFile> TakeVideoAsync(VideoMediaStorageOptions options)
        {
            throw new NotImplementedException();
        }
    }
}
