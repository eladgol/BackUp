#define WINDOWS
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Windows;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using XLabs.Platform.Device;
using AuthentiDocTake2.WindowsShared;

namespace AuthentiDocTake2.Windows
{
    public sealed partial class MainPage
    {
        public MainPage()
        {
            this.InitializeComponent();
            var h = this.ActualHeight;
            var w = this.ActualWidth;
            
            LoadApplication(new AuthentiDocTake2.App(new WindowsThreading(), new WindowsDevice(),  new WindowsSignalProcess(), new WindowsMediaPicker(new Size(400, 400)), new WindowsImageSupport(), new WindowsFileSupport()));
        }
    }
}
