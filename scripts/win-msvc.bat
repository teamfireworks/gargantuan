@echo off
:: Runs the command passed as arguments with the MSVC x64 build environment loaded.
::
:: cl.exe locates the CRT and Windows SDK headers through the INCLUDE environment
:: variable, which is set by vcvarsall.bat -- not by CMake and not by build.ninja.
:: Without it every compile fails with "fatal error C1083: Cannot open include file".
::
:: If the environment is already loaded (i.e. you are in a Native Tools prompt),
:: this is a pass-through and vcvarsall is not run again.
::
:: Usage: scripts\win-msvc.bat <command> [args...]

setlocal

if defined VCINSTALLDIR goto :run

:: Note: we pushd into the Installer directory and invoke vswhere.exe by bare name
:: rather than by full path. The path contains "Program Files (x86)", and cmd's
:: parser would treat the ")" as the end of the for(...) clause.
set "VSINSTALLER=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
pushd "%VSINSTALLER%" 2>nul
if errorlevel 1 goto :no_installer
if not exist "vswhere.exe" goto :no_vswhere

set "VSPATH="
for /f "usebackq tokens=*" %%i in (`.\vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%i"
popd

if not defined VSPATH goto :no_toolset
if not exist "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" goto :no_vcvars

call "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
if errorlevel 1 goto :vcvars_failed

:run
%*
exit /b %ERRORLEVEL%

:no_installer
echo [win-msvc] ERROR: Visual Studio Installer directory not found: 1>&2
echo [win-msvc]   "%VSINSTALLER%" 1>&2
echo [win-msvc] Is Visual Studio ^(or the Build Tools^) installed? 1>&2
exit /b 1

:no_vswhere
popd
echo [win-msvc] ERROR: vswhere.exe not found in the Visual Studio Installer directory. 1>&2
exit /b 1

:no_toolset
echo [win-msvc] ERROR: No Visual Studio installation with the C++ toolset was found. 1>&2
echo [win-msvc] Install the "Desktop development with C++" workload. 1>&2
exit /b 1

:no_vcvars
echo [win-msvc] ERROR: vcvarsall.bat missing under "%VSPATH%". 1>&2
exit /b 1

:vcvars_failed
echo [win-msvc] ERROR: vcvarsall.bat x64 failed. 1>&2
exit /b 1
