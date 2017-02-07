cd /d "C:\Code\MyCode\WindowsOpenCVComputeDLL\WindowsOpenCVComputeDLL" &msbuild "WindowsOpenCVComputeDLL.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform=x64
exit %errorlevel% 