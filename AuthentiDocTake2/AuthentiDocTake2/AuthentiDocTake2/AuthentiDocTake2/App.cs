using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Xamarin.Forms;
using XLabs.Platform.Services.Media;
using XLabs.Platform.Device;


namespace AuthentiDocTake2
{
	public class App : Application
	{ 
		public App (IThreadingCrossPlatform _IThreading, IDevice _currentDevice, SignalProcess _process = null, IMediaPicker _mediaPicker = null, IImageSupport _imageSupport = null, IFileSupport _fileSupport = null)
		{
            var navPage =
                new NavigationPage(
                    new AuthenticDocPage(_process, _IThreading, _currentDevice, _mediaPicker, _imageSupport, _fileSupport)
                    {
                        Title = "AuthenticDoc"
                    })
                    {
                        BarBackgroundColor = Color.FromHex("547799")
                    };
            MainPage = navPage;
        }

		protected override void OnStart ()
		{
			// Handle when your app starts
		}

		protected override void OnSleep ()
		{
			// Handle when your app sleeps
		}

		protected override void OnResume ()
		{
			// Handle when your app resumes
		}
	}
}
