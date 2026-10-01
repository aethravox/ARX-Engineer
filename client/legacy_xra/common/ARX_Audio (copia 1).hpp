// =============================================================
// ARX ENGINE - Modulo de Audio Avanzado (Raylib)
// Wrapper para usar Raylib como motor de audio unificado
// =============================================================
#ifndef ARX_AUDIO_HPP
#define ARX_AUDIO_HPP

#include "raylib.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace ARX {

// =============================================================
// ARXAudioEngine - Motor de audio basado en Raylib
// =============================================================
class ARXAudioEngine {
public:
    static inline bool initialized = false;
    static inline float masterVolume = 1.0f;

    static void Init() {
        if (initialized) return;

        InitAudioDevice();
        initialized = true;
        std::cout << "[ARXAudio] Motor de audio inicializado (Raylib Audio)" << std::endl;
    }

    static void Shutdown() {
        if (!initialized) return;
        CloseAudioDevice();
        initialized = false;
        std::cout << "[ARXAudio] Motor de audio cerrado" << std::endl;
    }

    static void SetMasterVolumeValue(float volume) {
        masterVolume = volume;
        if (initialized) {
            // Llamamos a la función global de Raylib directamente
            ::SetMasterVolume(volume); 
        }
    }

    static float GetMasterVolumeValue() {
        return masterVolume;
    }
};

// =============================================================
// ARXSound - Sonido individual (efectos cortos)
// =============================================================
class ARXSound {
public:
    std::string name;
    std::string filePath;
    bool loaded = false;

    ARXSound() = default;
    ~ARXSound() { Unload(); }

    ARXSound(ARXSound&& other) noexcept
        : name(std::move(other.name))
        , filePath(std::move(other.filePath))
        , loaded(other.loaded)
        , sound(other.sound)
    {
        other.loaded = false;
        other.sound = { 0 };
    }

    ARXSound& operator=(ARXSound&& other) noexcept {
        if (this != &other) {
            Unload();
            name = std::move(other.name);
            filePath = std::move(other.filePath);
            loaded = other.loaded;
            sound = other.sound;
            other.loaded = false;
            other.sound = { 0 };
        }
        return *this;
    }

    ARXSound(const ARXSound&) = delete;
    ARXSound& operator=(const ARXSound&) = delete;

    bool Load(const std::string& path) {
        ARXAudioEngine::Init();
        filePath = path;
        name = path;

        sound = LoadSound(path.c_str());
        // Verificación de carga correcta en Raylib
        if (sound.frameCount <= 0) {
            std::cout << "[ARXSound] Error cargando: " << path << std::endl;
            loaded = false;
            return false;
        }

        loaded = true;
        std::cout << "[ARXSound] Cargado: " << path << std::endl;
        return true;
    }

    void Play() { if (loaded) PlaySound(sound); }
    void Stop() { if (loaded) StopSound(sound); }
    void Pause() { if (loaded) PauseSound(sound); } // Corregido: antes llamaba a Stop
    void Resume() { if (loaded) ResumeSound(sound); }

    void SetVolume(float volume) { if (loaded) SetSoundVolume(sound, volume); }
    void SetPitch(float pitch) { if (loaded) SetSoundPitch(sound, pitch); }
    void SetPan(float pan) { if (loaded) SetSoundPan(sound, pan); }

    bool IsPlaying() const { return loaded && IsSoundPlaying(sound); }

    float GetDuration() const {
        if (!loaded || sound.stream.sampleRate <= 0) return 0.0f;
        return (float)sound.frameCount / (float)sound.stream.sampleRate;
    }

    void Unload() {
        if (loaded) {
            UnloadSound(sound);
            loaded = false;
        }
    }

private:
    Sound sound = { 0 };
};

// =============================================================
// ARXMusic - Musica de fondo (streaming)
// =============================================================
class ARXMusic {
public:
    std::string name;
    std::string filePath;
    bool loaded = false;

    ARXMusic() = default;
    ~ARXMusic() { Unload(); }

    ARXMusic(ARXMusic&& other) noexcept
        : name(std::move(other.name))
        , filePath(std::move(other.filePath))
        , loaded(other.loaded)
        , music(other.music)
    {
        other.loaded = false;
        other.music = { 0 };
    }

    ARXMusic& operator=(ARXMusic&& other) noexcept {
        if (this != &other) {
            Unload();
            name = std::move(other.name);
            filePath = std::move(other.filePath);
            loaded = other.loaded;
            music = other.music;
            other.loaded = false;
            other.music = { 0 };
        }
        return *this;
    }

    ARXMusic(const ARXMusic&) = delete;
    ARXMusic& operator=(const ARXMusic&) = delete;

    bool Load(const std::string& path) {
        ARXAudioEngine::Init();
        filePath = path;
        name = path;

        music = LoadMusicStream(path.c_str());
        if (music.frameCount <= 0) {
            std::cout << "[ARXMusic] Error cargando: " << path << std::endl;
            loaded = false;
            return false;
        }

        loaded = true;
        std::cout << "[ARXMusic] Cargado: " << path << std::endl;
        return true;
    }

    void Play() { if (loaded) PlayMusicStream(music); }
    void Stop() { if (loaded) StopMusicStream(music); }
    void Pause() { if (loaded) PauseMusicStream(music); }
    void Resume() { if (loaded) ResumeMusicStream(music); }
    void Update() { if (loaded) UpdateMusicStream(music); }

    void SetVolume(float volume) { if (loaded) SetMusicVolume(music, volume); }
    void SetPitch(float pitch) { if (loaded) SetMusicPitch(music, pitch); }
    void SetPan(float pan) { if (loaded) SetMusicPan(music, pan); }

    bool IsPlaying() const { return loaded && IsMusicStreamPlaying(music); }

    void SetLooping(bool loop) {
        if (!loaded) return;
        music.looping = loop;
    }

    void Unload() {
        if (loaded) {
            UnloadMusicStream(music);
            loaded = false;
        }
    }

private:
    Music music = { 0 };
};

// =============================================================
// ARXAudioManager - Gestor central
// =============================================
class ARXAudioManager {
public:
    static inline std::unordered_map<std::string, std::unique_ptr<ARXSound>> soundCache;
    static inline std::unordered_map<std::string, std::unique_ptr<ARXMusic>> musicCache;

    static ARXSound* LoadSound(const std::string& path) {
        if (soundCache.count(path)) return soundCache[path].get();
        auto snd = std::make_unique<ARXSound>();
        if (!snd->Load(path)) return nullptr;
        return (soundCache[path] = std::move(snd)).get();
    }

    static ARXMusic* LoadMusic(const std::string& path) {
        if (musicCache.count(path)) return musicCache[path].get();
        auto mus = std::make_unique<ARXMusic>();
        if (!mus->Load(path)) return nullptr;
        return (musicCache[path] = std::move(mus)).get();
    }

    static void PlaySound(const std::string& path, float volume = 1.0f) {
        ARXSound* snd = LoadSound(path);
        if (snd) { snd->SetVolume(volume); snd->Play(); }
    }

    static void PlayMusic(const std::string& path, float volume = 1.0f, bool loop = true) {
        ARXMusic* mus = LoadMusic(path);
        if (mus) {
            mus->SetVolume(volume);
            mus->SetLooping(loop);
            mus->Play();
        }
    }

    static void UpdateMusicStreams() {
        for (auto& pair : musicCache) { pair.second->Update(); }
    }

    static void ClearAll() {
        soundCache.clear();
        musicCache.clear();
        ARXAudioEngine::Shutdown();
    }
};

} // namespace ARX

#endif

