A basic SDL/BGFX app that works for iOS, macOS, Windows, Android and Linux using CMake and VCPKG.

The shaders need to be compiled with `compile.bat/compile.sh <platform>`. To see available platforms run shaderc(.exe). Compiling shaders should be done before creating the Xcode project with cmake so they are bundled with the iOS app.
`include/SDL` only needs to be used with Android to compile a shared version of the SDL library. CMAKE_TOOLCHAIN_FILE needs to be changed as well in the CMakeLists.txt to where your VCPKG is located for Android.

![image](https://github.com/user-attachments/assets/d274bfbd-7859-4777-a619-e2a2056f66bc)

The app includes a texture to load and a blue background to help while debugging. 
