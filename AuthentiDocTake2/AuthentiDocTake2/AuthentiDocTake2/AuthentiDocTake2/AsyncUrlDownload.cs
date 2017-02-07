using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net;
using System.IO;

namespace AuthentiDocTake2
{
    class AsyncUrlDownload
    {
        public IThreadingCrossPlatform m_IThreading;
        public AsyncUrlDownload(IThreadingCrossPlatform _IThreading)
        {
            m_IThreading = _IThreading;
        }
        //private List<byte> imageRaw = new List<byte>();
        private Stream imageRaw = new MemoryStream();
        private AutoResetEvent waitHandleForDownloadToEnd = new AutoResetEvent(false);
        async Task<bool> WaitOnHandle(System.Threading.WaitHandle waitHandle)
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "WaitOnHandle");
            bool bRecieved = waitHandleForDownloadToEnd.WaitOne(TimeSpan.FromMilliseconds(30000));

            threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "WaitOnHandle Finished");
            return bRecieved;
        }
        public async Task<Tuple<bool, Stream>> DownloadHttpToArray(string sUri)
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "DownloadHttpToArray");

            var request = WebRequest.Create(sUri);
            var asyncDownloadResult = request.BeginGetResponse(FeedDownloaded, request);
            bool bReceived = await WaitOnHandle(asyncDownloadResult.AsyncWaitHandle);
            Tuple<bool, Stream> Result = new Tuple<bool, Stream>(bReceived, imageRaw);
            return Result;
        }
        void FeedDownloaded(IAsyncResult result)
        {
            var threadID = m_IThreading.GetCurrentThreadID().ToString();
            System.Diagnostics.Debug.WriteLine(threadID + "FeedDownloaded");

            var request = result.AsyncState as HttpWebRequest;
            try
            { 
                var response = request.EndGetResponse(result);
                imageRaw = response.GetResponseStream();
                //var responseStream = response.GetResponseStream();
                //int streamCount;
                //do
                //{
                //    byte[] buf = new byte[1024];
                //    streamCount = responseStream.Read(buf, 0, 1024);
                //    imageRaw.AddRange(buf);
                //} while (responseStream.CanRead && streamCount > 0);
                waitHandleForDownloadToEnd.Set();
            }

            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(e);
            }
        }
    }
}
