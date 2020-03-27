set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24) # API level
set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
set(CMAKE_ANDROID_NDK D:/android-ndk-r12b-windows-x86/android-ndk-r12b)#D:/Android/ndk/android-ndk-r12b-windows-x86_64/android-ndk-r12b)
#set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK})
set(CMAKE_ANDROID_STL_TYPE gnustl_shared)
#set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION 4.9)
add_definitions(-DANDROID)
