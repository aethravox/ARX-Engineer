// ==============================================================================
// src/scene/audio/vox_audio.hpp — Voxes de audio.
//
// VoxAudioStreamPlayer — reproduce sonidos (WAV, MP3, OGG).
// VoxAudioListener — listener de audio (opcional, para 3D audio futuro).
//
// Usa AudioServer (singleton) directamente, no necesita SceneTree.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "audio/audio_server.hpp"

#include <string>
#include <memory>

namespace arx {

// VoxAudioStreamPlayer — reproduce un archivo de audio.
// Soporta WAV, MP3, OGG Vorbis. Volume, pitch, looping, autoplay.
class VoxAudioStreamPlayer : public Vox {
public:
    ARX_CLASS(VoxAudioStreamPlayer, Vox);
public:

    void _ready() override {
        if (autoplay_ && !stream_path_.empty()) {
            load(stream_path_);
            play();
        }
    }

    void _exit_tree() override {
        stop();
    }

    // Cargar archivo de audio
    bool load(const std::string& path) {
        stream_path_ = path;
        stream_ = AudioServer::instance().load(path);
        return stream_ && stream_->is_valid();
    }

    // Reproducir
    void play() {
        if (!stream_) {
            if (!stream_path_.empty()) load(stream_path_);
        }
        if (stream_) {
            source_ = AudioServer::instance().play(stream_, bus_name_, looping_);
            if (source_) {
                source_->volume = volume_;
                source_->pitch = pitch_;
            }
        }
    }

    void stop() {
        if (source_) {
            AudioServer::instance().stop(source_);
            source_.reset();
        }
    }

    void pause() {
        if (source_) AudioServer::instance().pause(source_);
    }

    void resume() {
        if (source_) AudioServer::instance().resume(source_);
    }

    bool is_playing() const {
        return source_ && source_->is_playing;
    }

    // Setters/getters
    void set_stream_path(const std::string& p) { stream_path_ = p; }
    const std::string& get_stream_path() const { return stream_path_; }

    void set_volume(float v) {
        volume_ = v;
        if (source_) source_->volume = v;
    }
    float get_volume() const { return volume_; }

    void set_pitch(float p) {
        pitch_ = p;
        if (source_) source_->pitch = p;
    }
    float get_pitch() const { return pitch_; }

    void set_looping(bool l) {
        looping_ = l;
        if (source_) source_->looping = l;
    }
    bool get_looping() const { return looping_; }

    void set_autoplay(bool a) { autoplay_ = a; }
    bool get_autoplay() const { return autoplay_; }

    void set_bus(const std::string& b) { bus_name_ = b; }
    const std::string& get_bus() const { return bus_name_; }

    float get_duration() const {
        return stream_ ? stream_->duration_seconds() : 0.0f;
    }

private:
    std::string stream_path_;
    std::shared_ptr<AudioStream> stream_;
    std::shared_ptr<AudioSource> source_;
    float volume_ = 1.0f;
    float pitch_ = 1.0f;
    bool looping_ = false;
    bool autoplay_ = false;
    std::string bus_name_ = "master";
};

} // namespace arx
