// ==============================================================================
// src/os/os.cpp — Factory según plataforma.
// ==============================================================================
#include "os/os.hpp"

#if defined(ARX_PLATFORM_LINUX) && ARX_PLATFORM_LINUX
    #include "platforms/linux/os_linux.hpp"
#elif defined(ARX_PLATFORM_WINDOWS) && ARX_PLATFORM_WINDOWS
    #include "platforms/windows/os_windows.hpp"
#elif defined(ARX_PLATFORM_WEB) && ARX_PLATFORM_WEB
    #include "platforms/web/os_web.hpp"
#elif defined(ARX_PLATFORM_ANDROID) && ARX_PLATFORM_ANDROID
    #include "platforms/android/os_android.hpp"
#endif

namespace arx {

OS* OS::create() {
#if defined(ARX_PLATFORM_LINUX) && ARX_PLATFORM_LINUX
    return new OSLinux();
#elif defined(ARX_PLATFORM_WINDOWS) && ARX_PLATFORM_WINDOWS
    return new OSWindows();
#elif defined(ARX_PLATFORM_WEB) && ARX_PLATFORM_WEB
    return new OSWeb();
#elif defined(ARX_PLATFORM_ANDROID) && ARX_PLATFORM_ANDROID
    return new OSAndroid();
#else
    // Unknown platform - return null so caller can check
    return nullptr;
#endif
}

} // namespace arx
