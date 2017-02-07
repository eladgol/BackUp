using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO;
using XLabs.Platform.Device;
using Xamarin.Forms;
using XLabs.Platform.Services.Media;
using XLabs.Ioc;
using System.Net;
using OpenTK;
using OpenTK.Graphics;
using OpenTK.Graphics.ES20;
namespace AuthentiDocTake2
{
    public partial class AuthenticDocPage : ContentPage
    {
        AuthenticDocModel model;
        public int m_framePerSec = 4;
        private CancellationTokenSource m_cancellationToken;
        private Rectangle m_rect;
        public AuthenticDocPage(SignalProcess _process, IThreadingCrossPlatform _IThreading, IDevice _currentDevice, IMediaPicker _mediaPicker = null, IImageSupport _imageSupport = null, IFileSupport _fileSupport = null)
        {
            string[] testURls = {"http://www.atmospheric-research.com/websops/Scanned%20Documents/Document%20(37).jpg",
                                   "http://www.bcot1.com/test1.bmp"};
            model = new AuthenticDocModel(_process, _IThreading, _currentDevice, _mediaPicker, _imageSupport, _fileSupport, testURls);
            BindingContext = model;

            InitializeComponent();
          
            foreach(var sUrl in testURls)
                URLEntryPicker.Items.Add(sUrl);
            URLEntry.Text = URLEntryPicker.Items[0];
            URLEntry.TextColor = Color.Black;

            //openGLView.HasRenderLoop = true;
            //openGLView.HeightRequest = 300;
            //openGLView.WidthRequest = 300;
            //openGLView.OnDisplay = r =>
            //{
            //    Random rnd = new Random();
            //    OpenTK.Graphics.ES20.GL.ClearColor((float)rnd.NextDouble(), (float)rnd.NextDouble(), (float)rnd.NextDouble(), 0.7f);
            //    GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            //};
            //testButton.Clicked += (s, a) => { this.openGLView.Display(); };
            //openGLView.OnDisplay = display;
            //testButton.Clicked += clicked;

            //this.m_cancellationToken = new CancellationTokenSource();
            //Timer is Stopped
            //TimerRunning(this.m_cancellationToken.Token);
            //m_rect = new Rectangle(100, 100, 300, 300);

            //GL.Viewport(100, 100, 500, 500);
            //display(m_rect);
            URLEntryPicker.SelectedIndexChanged += (s, a) => { URLEntry.Text = testURls[URLEntryPicker.SelectedIndex]; };
        }
        
        //void display(Rectangle r)
        //{
            //Random rnd = new Random();
            //OpenTK.Graphics.ES20.GL.ClearColor((float)rnd.NextDouble(), (float)rnd.NextDouble(), (float)rnd.NextDouble(), 0.7f);
            //GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
        
        //}
        //void clicked(object sender, EventArgs e)
        //{
        //    this.openGLView.Display();

        //}
        //public async void TimerRunning(CancellationToken token)
        //{
        //    //var startTime = DateTime.Now;
        //    //var watch = Stopwatch.StartNew();
        //    while (!token.IsCancellationRequested)
        //    {
        //        try
        //        {
        //            await Task.Delay((int)((1.0 / m_framePerSec) * 1000), token);
        //        }
        //        catch (TaskCanceledException)
        //        {
        //        }
        //        try
        //        {
        //            Device.BeginInvokeOnMainThread(() => this.openGLView.Display());
        //        }
        //        catch (Exception e)
        //        {

        //        }
                
              
        //    }
        //}
    }
}