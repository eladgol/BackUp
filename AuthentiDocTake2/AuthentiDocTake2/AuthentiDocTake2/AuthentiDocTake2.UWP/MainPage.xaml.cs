using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using AuthentiDocTake2.WindowsShared;
using XLabs.Platform.Device;

namespace AuthentiDocTake2.UWP
{
    public sealed partial class MainPage
    {
        public MainPage()
        {
            this.InitializeComponent();
            var _threading = new WindowsThreading();
            //var _device = new XLabs.Platform.Device.WindowsDevice();
            var _signalProcess = new WindowsSignalProcess();
            var _mediaPicker = new WindowsMediaPicker(new Size(400, 400));
            LoadApplication(new AuthentiDocTake2.App(_threading, null, _signalProcess, _mediaPicker));
        }
    }
}
