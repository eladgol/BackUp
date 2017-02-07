//using System;
//using System.Collections.Generic;
//using System.Text;
//using System.IO;
//using Xamarin.Forms;
//namespace AuthentiDocTake2
//{
//    class StreamableImageSource : BindableObject
//    {
//        private byte[] ImageRaw { get; set; }

//        public Stream ImageStream 
//        {
//            get { return ImageRaw != null ? new MemoryStream(ImageRaw) : null; }
//            set
//            {
//                if (value != null)
//                {
//                    ImageRaw = new byte[value.Length];
//                    value.Read(ImageRaw, 0, ImageRaw.Length);

//                    SetValue(ImageSourceProperty, ImageSource.FromStream(() => new MemoryStream(ImageRaw)));
//                }
//            }
//        }

//        public static readonly BindableProperty ImageSourceProperty =
//            BindableProperty.Create<StreamableImageSource, ImageSource>(p => p.ImageSource,
//                ImageSource.FromUri(new Uri("http://www.nasa.gov/sites/default/files/styles/1600x1200_autoletterbox/public/images/298773main_EC02-0282-3_full.jpg")));

//        public ImageSource ImageSource
//        {
//            get
//            {
//                return ImageStream != null
//                    ? ImageSource.FromStream(() => ImageStream)
//                    : (ImageSource)GetValue(ImageSourceProperty);
//            }
//            set
//            {
//                //ImageRaw = null;
//                //SetValue(ImageSourceProperty, value);
//                ImageSource = value;
//                Func<byte[]> a = () => {
//                    using (MemoryStream ms = new MemoryStream())
//                    {
//                        ImageSource. .CopyTo(ms);
//                        return ms.ToArray();
//                    }
//                };
//            }
//        }

//        public static implicit operator Image(StreamableImageSource v)
//        {
//            throw new NotImplementedException();
//        }
//    }
//}
