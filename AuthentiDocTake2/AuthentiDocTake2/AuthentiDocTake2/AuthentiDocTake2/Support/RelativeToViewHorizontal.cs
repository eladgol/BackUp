using System;
using System.Collections.Generic;
using System.Text;

using Xamarin.Forms;

namespace AuthentiDocTake2.Support
{
    public class RelativeToViewHorizontal : RelativeToView
    {
        protected override double DetermineExtent(VisualElement view)
        {
            return view.Width;
        }

        protected override double DetermineStart(VisualElement view)
        {
            return view.X;
        }
    }
}
