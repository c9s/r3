@ECHO off
SETLOCAL ENABLEDELAYEDEXPANSION
:: usage:
::          build.cmd <config> <arch>
::                  <config> - configuration to be used for build (default: Debug)
::                  <arch> - x64 or x86 (default: x64)
if NOT "%1" == "" (set CMAKE_BUILD_TYPE=%1) else (set CMAKE_BUILD_TYPE=Debug)
if NOT "%2" == "" (set BUILD_TARGET_ARCH=%2) else (set BUILD_TARGET_ARCH=x64)


if "%VCPKG_HOME%" == "" (
    set VCPKG_HOME=D:/Development/c/tools/vcpkg
)


set VCPKG_TOOLCHAIN=scripts/buildsystems/vcpkg.cmake
set CMAKE_BINARY_DIR=build/%BUILD_TARGET_ARCH%
set TARGET_PLATFORM=8.1


if "%BUILD_TARGET_ARCH%" == "x64" (
    set GENERATOR_NAME="Visual Studio 14 2015 Win64"
    set BUILD_TARGET_TRIPLET=x64-windows
) else (
    set GENERATOR_NAME="Visual Studio 14 2015"
    set BUILD_TARGET_TRIPLET=x86-windows
)

IF NOT EXIST "%CMAKE_BINARY_DIR%\*.sln" (
    cmake -H"." -B"%CMAKE_BINARY_DIR%" -G%GENERATOR_NAME% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_HOME%/%VCPKG_TOOLCHAIN% -DVCPKG_TARGET_TRIPLET=%BUILD_TARGET_TRIPLET% -DCMAKE_SYSTEM_VERSION=%TARGET_PLATFORM%
)
cmake --build "%CMAKE_BINARY_DIR%" --target ALL_BUILD --config "%CMAKE_BUILD_TYPE%"
copy "%VCPKG_HOME%\installed\%BUILD_TARGET_TRIPLET%\Debug\bin\pcred.dll" "%cd%\build\%BUILD_TARGET_ARCH%\tests\%CMAKE_BUILD_TYPE%\pcre.dll"
copy "%VCPKG_HOME%\installed\%BUILD_TARGET_TRIPLET%\Debug\bin\pcred.pdb" "%cd%\build\%BUILD_TARGET_ARCH%\tests\%CMAKE_BUILD_TYPE%\pcre.pdb"
cmake --build "%CMAKE_BINARY_DIR%" --target RUN_TESTS --config "%CMAKE_BUILD_TYPE%"

ENDLOCAL
