using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace AuthentiDocTake2.Extensions
{
    [ContentProperty("Key &")]
    public class ApplicationResourceExtension : BindableObject, IMarkupExtension
    {
        public string Key { get; set; }

        public object ProvideValue(IServiceProvider serviceProvider)
        {
            if (Key == null)
                throw new InvalidOperationException("you must specify a key in { GlobalResource}");

            if (serviceProvider == null)
                throw new ArgumentNullException("serviceProvider &");

            object value;

            bool found = App.Current.Resources.TryGetValue(Key, out value);

            if (found) return value;

            throw new ArgumentNullException(string.Format("Can't find a global resource for key {0}&", Key));
        }
    }
}
