// ==============================================================================
// src/audio/audio_server.hpp — Servidor de audio.
// Carga y reproduce sonidos (WAV, MP3, OGG Vorbis). Mezcla varios streams
// con volumen y pitch independientes.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <thread>

namespace arx {

// AudioStream — datos de audio decodificados en memoria (PCM float32).
struct AudioStream {
    std::vector<float>  samples;        // Interleaved si stereo
    int                 channels    = 0;
    int                 sample_rate = 0;
    int64_t             frame_count = 0;

    bool is_valid() const { return !samples.empty() && channels > 0 && sample_rate > 0; }
    float duration_seconds() const {
        return is_valid() ? static_cast<float>(frame_count) / sample_rate : 0.0f;
    }
};

// AudioSource — instancia reproducible de un AudioStream.
struct AudioSource {
    std::shared_ptr<AudioStream> stream;
    float    volume       = 1.0f;
    float    pitch        = 1.0f;
    bool     looping      = false;
    bool     is_playing   = false;
    bool     is_paused    = false;
    int64_t  play_cursor  = 0;          // En frames
    float    fade_in      = 0.0f;       // Segundos
    float    fade_out     = 0.0f;
    float    elapsed      = 0.0f;

    // Mixer puede escribir aquí cada frame.
    float    current_volume = 1.0f;
};

// Bus — grupo de fuentes (master, music, sfx, voice, etc.).
struct AudioBus {
    std::string name;
    float       volume = 1.0f;
    bool        muted  = false;
    bool        solo   = false;
    std::vector<std::shared_ptr<AudioSource>> sources;
};

// AudioServer — singleton de audio.
class AudioServer {
public:
    static AudioServer& instance();

    bool init(int sample_rate = 44100, int channels = 2, int buffer_size = 1024);
    void shutdown();

    // Carga de archivos.
    std::shared_ptr<AudioStream> load_wav(const std::string& path);
    std::shared_ptr<AudioStream> load_mp3(const std::string& path);
    std::shared_ptr<AudioStream> load_ogg(const std::string& path);
    std::shared_ptr<AudioStream> load(const std::string& path);  // Detecta por extensión.

    // Playback.
    std::shared_ptr<AudioSource> play(std::shared_ptr<AudioStream> stream,
                                   const std::string& bus_name = "master",
                                   bool looping = false);
    void stop(std::shared_ptr<AudioSource> src);
    void pause(std::shared_ptr<AudioSource> src);
    void resume(std::shared_ptr<AudioSource> src);

    void set_master_volume(float v);
    float get_master_volume() const { return master_volume_; }

    // Buses.
    AudioBus* get_bus(const std::string& name);
    AudioBus* create_bus(const std::string& name);
    void      set_bus_volume(const std::string& name, float v);
    void      set_bus_muted(const std::string& name, bool m);
    void      set_bus_solo(const std::string& name, bool s);

    // Mixer loop — llamado por el backend de audio (ALSA/PulseAudio/miniaudio).
    void mix(float* output, int frame_count, int channels);

    // Stats.
    int active_voice_count() const;

private:
    AudioServer() = default;
    int      sample_rate_  = 44100;
    int      channels_     = 2;
    int      buffer_size_  = 1024;
    float    master_volume_ = 1.0f;
    bool     initialized_  = false;
    std::vector<AudioBus> buses_;
    mutable std::mutex mtx_;

    void mix_source(AudioSource& src, float* out, int frames, int channels);
};

// Helper — wrapper OO para ARXScript.
class Audio : public Object {
public:
    ARX_CLASS(Audio, Object);
public:

    static std::shared_ptr<AudioStream> load(const std::string& path) {
        return AudioServer::instance().load(path);
    }
    static std::shared_ptr<AudioSource> play(std::shared_ptr<AudioStream> s,
                                          bool loop = false) {
        return AudioServer::instance().play(s, "master", loop);
    }
    static void set_master_volume(float v) { AudioServer::instance().set_master_volume(v); }
};

} // namespace arx
