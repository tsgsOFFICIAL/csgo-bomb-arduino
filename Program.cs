using System;
using System.Media;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.Threading;

namespace ArduinoBombSerialConnection
{
    class Program
    {
        private static SerialPort _serialPort = new SerialPort();
        #region Audio
        private const byte VK_VOLUME_MUTE = 0xAD;
        private const byte VK_VOLUME_DOWN = 0xAE;
        private const byte VK_VOLUME_UP = 0xAF;
        private const UInt32 KEYEVENTF_EXTENDEDKEY = 0x0001;
        private const UInt32 KEYEVENTF_KEYUP = 0x0002;

        [DllImport("user32.dll")]
        static extern void keybd_event(byte bVk, byte bScan, UInt32 dwFlags, UInt32 dwExtraInfo);

        [DllImport("user32.dll")]
        static extern Byte MapVirtualKey(UInt32 uCode, UInt32 uMapType);

        [ComImport]
        [Guid("A95664D2-9614-4F35-A746-DE8DB63617E6"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IMMDeviceEnumerator
        {
            void _VtblGap1_1();
            int GetDefaultAudioEndpoint(int dataFlow, int role, out IMMDevice ppDevice);
        }
        private static class MMDeviceEnumeratorFactory
        {
            public static IMMDeviceEnumerator CreateInstance()
            {
                return (IMMDeviceEnumerator)Activator.CreateInstance(Type.GetTypeFromCLSID(new Guid("BCDE0395-E52F-467C-8E3D-C4579291692E"))); //a MMDeviceEnumerator
            }
        }

        [Guid("D666063F-1587-4E43-81F1-B948E807363F"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IMMDevice
        {
            int Activate([MarshalAs(UnmanagedType.LPStruct)] Guid iid, int dwClsCtx, IntPtr pActivationParams, [MarshalAs(UnmanagedType.IUnknown)] out object ppInterface);
        }

        [Guid("5CDF2C82-841E-4546-9722-0CF74078229A"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        public interface IAudioEndpointVolume
        {
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RegisterControlChangeNotify(/* [in] */__in IAudioEndpointVolumeCallback *pNotify) = 0;
            int RegisterControlChangeNotify(IntPtr pNotify);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UnregisterControlChangeNotify(/* [in] */ __in IAudioEndpointVolumeCallback *pNotify) = 0;
            int UnregisterControlChangeNotify(IntPtr pNotify);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetChannelCount(/* [out] */ __out UINT *pnChannelCount) = 0;
            int GetChannelCount(ref uint pnChannelCount);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMasterVolumeLevel( /* [in] */ __in float fLevelDB,/* [unique][in] */ LPCGUID pguidEventContext) = 0;
            int SetMasterVolumeLevel(float fLevelDB, Guid pguidEventContext);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMasterVolumeLevelScalar( /* [in] */ __in float fLevel,/* [unique][in] */ LPCGUID pguidEventContext) = 0;
            int SetMasterVolumeLevelScalar(float fLevel, Guid pguidEventContext);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMasterVolumeLevel(/* [out] */ __out float *pfLevelDB) = 0;
            int GetMasterVolumeLevel(ref float pfLevelDB);
            //virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMasterVolumeLevelScalar( /* [out] */ __out float *pfLevel) = 0;
            int GetMasterVolumeLevelScalar(ref float pfLevel);
        }
        #endregion
        static void Main(string[] args)
        {
            if (args.Length >= 1)
            {
                _serialPort.PortName = args[0].ToUpper();
            }
            else
            {
                _serialPort.PortName = "COM4";
            }
            _serialPort.BaudRate = 9600;
            bool serialOpen = false;

            while (!serialOpen)
            {
                try
                {
                    _serialPort.Open();
                    serialOpen = true;
                }
                catch (Exception)
                { }
            }

            string lastCommand = "";
            while (true)
            {
                if (!_serialPort.IsOpen)
                {
                    serialOpen = false;
                    while (!serialOpen)
                    {
                        try
                        {
                            _serialPort.Open();
                            serialOpen = true;
                        }
                        catch (Exception)
                        { }
                    }
                }
                Thread.Sleep(100);
                try
                {
                    string command = _serialPort.ReadExisting();
                    if (!command.Equals(lastCommand) && !command.Trim().Equals(""))
                        Console.WriteLine(command);
                    {
                        lastCommand = command;
                        // Console.WriteLine($"\"{command}\""); // Debugging

                        switch (command)
                        {
                            case "planted":
                                PlaySound("plant.wav");
                                break;
                            case "defused":
                                PlaySound("defuse.wav");
                                break;
                            case "ctwin":
                                PlaySound("ctwin.wav");
                                break;
                            case "twin":
                                PlaySound("twin.wav");
                                Thread.Sleep(2200);
                                Environment.Exit(0);
                                break;
                            case "explode":
                                PlaySound("explode.wav");
                                Thread.Sleep(2200);
                                break;
                            default:
                                break;
                        }
                    }
                }
                catch (Exception)
                { }
            }
        }

        /// <summary>
        /// Plays a certain sound on the victims pc
        /// </summary>
        /// <param name="sound"></param>
        private static void PlaySound(string sound, string volume = "100")
        {
            SetVolume(volume); //Set volume

            try
            {
                SoundPlayer sp = new SoundPlayer();
                sp.SoundLocation = $@"sound\{sound}";
                sp.Play();
                sp.Dispose();
            }
            catch (Exception)
            { }
        }
        /// <summary>
        /// Set the volume on the victims pc
        /// </summary>
        /// <param name="percentage">0-100</param>
        private static void SetVolume(string percentage)
        {
            try
            {
                float volume = float.Parse(percentage) / 100;

                keybd_event(VK_VOLUME_UP, MapVirtualKey(VK_VOLUME_UP, 0), KEYEVENTF_EXTENDEDKEY, 0);
                keybd_event(VK_VOLUME_UP, MapVirtualKey(VK_VOLUME_UP, 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

                IMMDeviceEnumerator deviceEnumerator = MMDeviceEnumeratorFactory.CreateInstance();
                IMMDevice speakers;
                const int eRender = 0;
                const int eMultimedia = 1;
                deviceEnumerator.GetDefaultAudioEndpoint(eRender, eMultimedia, out speakers);

                object aepv_obj;
                speakers.Activate(typeof(IAudioEndpointVolume).GUID, 0, IntPtr.Zero, out aepv_obj);
                IAudioEndpointVolume aepv = (IAudioEndpointVolume)aepv_obj;
                Guid ZeroGuid = new Guid();
                int res = aepv.SetMasterVolumeLevelScalar(volume, ZeroGuid);
            }
            catch (Exception) { }
        }
    }
}
