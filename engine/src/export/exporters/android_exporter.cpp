// ==============================================================================
// src/export/exporters/android_exporter.cpp
// ==============================================================================
#include "android_exporter.hpp"
#include "core/logging.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace arx {

bool AndroidExporter::compile_runtime(const fs::path& root,
                                        const ExportPreset& preset,
                                        ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path gradle_dir = out_dir / "gradle";
    fs::create_directories(gradle_dir);

    // 1. Generar proyecto Gradle completo.
    write_gradle_project_(gradle_dir, preset);
    write_android_manifest_(gradle_dir, preset);
    write_main_activity_(gradle_dir, preset);

    // 2. Copiar fuentes ARX transpiladas al proyecto Gradle.
    fs::path gen_src = gradle_dir / "app" / "src" / "main" / "cpp" / "arx_generated";
    fs::create_directories(gen_src);
    fs::path arx_gen = out_dir / "arx_generated";
    if (fs::exists(arx_gen)) {
        copy_directory_recursive(arx_gen, gen_src);
    }

    // 3. Ejecutar gradlew assembleRelease (que internamente invoca al NDK).
    std::string gradlew = (gradle_dir / "gradlew").string();
#ifdef _WIN32
    gradlew += ".bat";
#endif
    std::string cmd = "cd \"" + gradle_dir.string() + "\" && " + gradlew +
                      " assembleRelease";
    result.logs.push_back("Ejecutando: " + cmd);
    int rc = std::system(cmd.c_str());
    if (rc != 0) {
        result.error_message = "Gradle build fallido";
        return false;
    }

    result.source_files_compiled++;
    return true;
}

bool AndroidExporter::package_output(const fs::path& root,
                                       const ExportPreset& preset,
                                       ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path apk_dir = out_dir / "gradle" / "app" / "build" / "outputs" / "apk" / "release";
    fs::path apk = apk_dir / ("app-release-unsigned.apk");
    if (!fs::exists(apk)) {
        // Fallback a debug.
        apk = out_dir / "gradle" / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk";
    }
    if (!fs::exists(apk)) {
        result.error_message = "No se generó el APK";
        return false;
    }

    fs::path final = out_dir / (preset.app_name + ".apk");
    fs::copy_file(apk, final, fs::copy_options::overwrite_existing);

    result.output_path = final.string();
    result.total_bytes = fs::file_size(final);
    return true;
}

void AndroidExporter::write_gradle_project_(const fs::path& dir,
                                              const ExportPreset& preset) {
    fs::create_directories(dir / "app" / "src" / "main" / "cpp");
    fs::create_directories(dir / "app" / "src" / "main" / "res" / "values");
    fs::create_directories(dir / "app" / "src" / "main" / "assets");

    // settings.gradle
    std::ofstream(dir / "settings.gradle") << "include ':app'\n";

    // build.gradle (root)
    std::ofstream(dir / "build.gradle") << R"(
buildscript {
    repositories { google(); mavenCentral() }
    dependencies { classpath 'com.android.tools.build:gradle:8.1.0' }
}
allprojects {
    repositories { google(); mavenCentral() }
}
)";

    // app/build.gradle
    std::ofstream(dir / "app" / "build.gradle") << R"(
apply plugin: 'com.android.application'
android {
    compileSdk 34
    ndkVersion "25.2.9519653"
    defaultConfig {
        applicationId ")" << preset.android_package << R"("
        minSdk )" << preset.android_min_sdk << R"(
        targetSdk 34
        versionCode 1
        versionName ")" << preset.app_version << R"("
        ndk { abiFilters 'arm64-v8a', 'armeabi-v7a' }
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++20 -frtti -fexceptions"
                arguments "-DARX_PLATFORM_ANDROID=ON",
                          "-DARX_BUILD_EDITOR=OFF"
            }
        }
    }
    externalNativeBuild {
        cmake { path "src/main/cpp/CMakeLists.txt" }
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'),
                          'proguard-rules.pro'
        }
    }
}
dependencies {}
)";

    // CMakeLists para el NDK
    std::ofstream(dir / "app" / "src" / "main" / "cpp" / "CMakeLists.txt") << R"(
cmake_minimum_required(VERSION 3.20)
project(arx_app CXX)
set(CMAKE_CXX_STANDARD 20)

# ARX core (submodule al repo del motor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.. arx_core)

file(GLOB ARX_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/arx_generated/*.cpp)

add_library(arx_app SHARED
    main_exported.cpp
    ${ARX_GENERATED}
)
target_link_libraries(arx_app PRIVATE arx_core log android EGL GLESv3)
)";

    // main_exported.cpp (JNI entry point)
    std::ofstream(dir / "app" / "src" / "main" / "cpp" / "main_exported.cpp") << R"(
#include <jni.h>
#include <android/native_window_jni.h>
#include "core/types.hpp"
#include "core/logging.hpp"
#include "scene/scene_tree.hpp"
#include "render/renderer.hpp"

namespace arx_script { void register_all_classes(); }

extern "C" JNIEXPORT void JNICALL
Java_com_arx_app_ARXActivity_nativeInit(JNIEnv* env, jobject thiz) {
    ::arx::arx_script::register_all_classes();
    // Aquí arrancaría el SceneTree sobre el ANativeWindow recibido.
    // En una implementación completa: extraer assets del APK via AAssetManager,
    // inicializar OpenGL ES 3, entrar en el loop de render en otro thread.
}
)";

    // gradle.properties
    std::ofstream(dir / "gradle.properties") << R"(
android.useAndroidX=true
org.gradle.jvmargs=-Xmx2048m
)";
}

void AndroidExporter::write_android_manifest_(const fs::path& dir,
                                                  const ExportPreset& preset) {
    std::ofstream(dir / "app" / "src" / "main" / "AndroidManifest.xml")
        << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package=")" << preset.android_package << R"(">
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.VIBRATE" />
    <uses-feature android:glEsVersion="0x00030000" android:required="true" />
    <application
        android:label=")" << preset.app_name << R"("
        android:icon="@mipmap/ic_launcher"
        android:hasCode="true"
        android:theme="@android:style/Theme.NoTitleBar.Fullscreen">
        <activity android:name=".ARXActivity"
                  android:configChanges="orientation|screenSize|keyboardHidden"
                  android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
)";
    // strings.xml con app_name
    std::ofstream(dir / "app" / "src" / "main" / "res" / "values" / "strings.xml")
        << "<resources>\n  <string name=\"app_name\">"
        << preset.app_name << "</string>\n</resources>\n";
}

void AndroidExporter::write_main_activity_(const fs::path& dir,
                                              const ExportPreset& preset) {
    fs::path pkg = dir / "app" / "src" / "main" / "java" / "com" / "arx" / "app";
    fs::create_directories(pkg);
    std::ofstream(pkg / "ARXActivity.java") << R"(
package com.arx.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.WindowManager;

public class ARXActivity extends Activity {
    static {
        System.loadLibrary("arx_app");
    }
    public native void nativeInit();

    @Override
    protected void onCreate(Bundle s) {
        super.onCreate(s);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        nativeInit();
    }
}
)";
}

} // namespace arx
