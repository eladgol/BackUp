using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;
namespace AuthentiDocTake2.Support
{
    class RelativeToViewVertical : RelativeToView
    {
        protected override double DetermineExtent(VisualElement view)
        {
            return view.Height;
        }

        protected override double DetermineStart(VisualElement view)
        {
            return view.Y;
        }
    }
}
