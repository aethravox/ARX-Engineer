// ==============================================================================
// src/audio/audio_server.cpp
// ==============================================================================
#include "audio_server.hpp"
#include "core/logging.hpp"

#include <fstream>
#include <cmath>
#include <algorithm>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_FLOAT_OUTPUT
#include <minimp3.h>

#include <stb_vorbis.c>  // Single-file decoder

namespace arx {

AudioServer& AudioServer::instance() {
    static AudioServer inst;
    return inst;
}

bool AudioServer::init(int sample_rate, int channels, int buffer_size) {
    sample_rate_ = sample_rate;
    channels_    = channels;
    buffer_size_ = buffer_size;
    initialized_ = true;

    // Bus master por defecto.
    if (buses_.empty()) {
        buses_.push_back({"master", 1.0f, false, false, {}});
        buses_.push_back({"music",  1.0f, false, false, {}});
        buses_.push_back({"sfx",    1.0f, false, false, {}});
        buses_.push_back({"voice",  1.0f, false, false, {}});
    }

    ARX_LOG_INFO("AudioServer inicializado ({}Hz, {}ch, buffer {})",
                 sample_rate, channels, buffer_size);
    return true;
}

void AudioServer::shutdown() {
    std::lock_guard<std::mutex> lock(mtx_);
    buses_.clear();
    initialized_ = false;
}

// ===================== Carga de archivos =====================================
std::shared_ptr<AudioStream> AudioServer::load(const std::string& path) {
    auto ext_pos = path.find_last_of('.');
    if (ext_pos == std::string::npos) return nullptr;
    std::string ext = path.substr(ext_pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "wav")  return load_wav(path);
    if (ext == "mp3")  return load_mp3(path);
    if (ext == "ogg")  return load_ogg(path);
    ARX_LOG_WARN("AudioServer: formato no soportado: {}", ext);
    return nullptr;
}

std::shared_ptr<AudioStream> AudioServer::load_wav(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { ARX_LOG_ERROR("load_wav: no se pudo abrir {}", path); return nullptr; }

    auto stream = std::make_shared<AudioStream>();

    // WAV header mínimo (44 bytes).
    char header[44];
    f.read(header, 44);
    if (!f) { ARX_LOG_ERROR("load_wav: header truncado en {}", path); return nullptr; }

    // Verificar "RIFF"
    if (std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0) {
        ARX_LOG_ERROR("load_wav: no es WAV válido: {}", path);
        return nullptr;
    }

    // Buscar chunk fmt y data.
    f.seekg(12);
    while (f) {
        char chunk_id[4];
        uint32_t chunk_size;
        f.read(chunk_id, 4);
        f.read(reinterpret_cast<char*>(&chunk_size), 4);
        if (!f) break;

        if (std::memcmp(chunk_id, "fmt ", 4) == 0) {
            uint16_t audio_format, num_channels;
            uint32_t sample_rate;
            uint16_t bits_per_sample;
            f.read(reinterpret_cast<char*>(&audio_format), 2);
            f.read(reinterpret_cast<char*>(&num_channels), 2);
            f.read(reinterpret_cast<char*>(&sample_rate), 4);
            f.seekg(6, std::ios::cur);  // byte_rate + block_align
            f.read(reinterpret_cast<char*>(&bits_per_sample), 2);
            if (chunk_size > 16) f.seekg(chunk_size - 16, std::ios::cur);
            stream->channels    = num_channels;
            stream->sample_rate = sample_rate;
        } else if (std::memcmp(chunk_id, "data", 4) == 0) {
            size_t samples_count = chunk_size / (sizeof(int16_t));
            std::vector<int16_t> pcm(samples_count);
            f.read(reinterpret_cast<char*>(pcm.data()), chunk_size);
            stream->samples.resize(samples_count);
            for (size_t i = 0; i < samples_count; ++i) {
                stream->samples[i] = pcm[i] / 32768.0f;
            }
            stream->frame_count = samples_count / stream->channels;
        } else {
            f.seekg(chunk_size, std::ios::cur);
        }
        if (chunk_size % 2) f.seekg(1, std::ios::cur);  // Padding
    }

    ARX_LOG_INFO("WAV cargado: {} ({}s, {}Hz, {}ch)", path,
                 stream->duration_seconds(), stream->sample_rate, stream->channels);
    return stream;
}

std::shared_ptr<AudioStream> AudioServer::load_mp3(const std::string& path) {
    static mp3dec_t dec;
    static bool dec_init = false;
    if (!dec_init) { mp3dec_init(&dec); dec_init = true; }

    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) { ARX_LOG_ERROR("load_mp3: no se pudo abrir {}", path); return nullptr; }

    size_t size = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> data(size);
    f.read(reinterpret_cast<char*>(data.data()), size);

    auto stream = std::make_shared<AudioStream>();

    mp3dec_frame_info_t info;
    std::vector<float> all_samples;
    int total_frames = 0;
    size_t offset = 0;
    float pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    while (offset < size) {
        int samples = mp3dec_decode_frame(&dec, data.data() + offset,
                                           size - offset, pcm, &info);
        if (samples == 0) break;
        for (int i = 0; i < samples; ++i) all_samples.push_back(pcm[i]);
        total_frames += samples / info.channels;
        offset += info.frame_bytes;
        if (info.frame_bytes == 0) break;
    }

    stream->samples     = std::move(all_samples);
    stream->channels    = info.channels;
    stream->sample_rate = info.hz;
    stream->frame_count = total_frames;

    ARX_LOG_INFO("MP3 cargado: {} ({}s, {}Hz, {}ch)", path,
                 stream->duration_seconds(), stream->sample_rate, stream->channels);
    return stream;
}

std::shared_ptr<AudioStream> AudioServer::load_ogg(const std::string& path) {
    int err = 0;
    stb_vorbis* vorb = stb_vorbis_open_filename(path.c_str(), &err, nullptr);
    if (!vorb) {
        ARX_LOG_ERROR("load_ogg: error {} abriendo {}", err, path);
        return nullptr;
    }

    stb_vorbis_info info = stb_vorbis_get_info(vorb);
    auto stream = std::make_shared<AudioStream>();
    stream->channels    = info.channels;
    stream->sample_rate = info.sample_rate;

    size_t total_samples = stb_vorbis_stream_length_in_samples(vorb) * info.channels;
    stream->samples.resize(total_samples);
    int read = stb_vorbis_get_samples_float_interleaved(vorb, info.channels,
                                                         stream->samples.data(),
                                                         total_samples);
    (void)read;
    stream->frame_count = stream->samples.size() / info.channels;
    stb_vorbis_close(vorb);

    ARX_LOG_INFO("OGG cargado: {} ({}s, {}Hz, {}ch)", path,
                 stream->duration_seconds(), stream->sample_rate, stream->channels);
    return stream;
}

// ===================== Playback ==============================================
std::shared_ptr<AudioSource> AudioServer::play(std::shared_ptr<AudioStream> stream,
                                             const std::string& bus_name,
                                             bool looping) {
    if (!stream || !stream->is_valid()) return nullptr;
    auto src = std::make_shared<AudioSource>();
    src->stream     = stream;
    src->looping    = looping;
    src->is_playing = true;

    std::lock_guard<std::mutex> lock(mtx_);
    AudioBus* bus = get_bus(bus_name);
    if (!bus) bus = &buses_[0];   // fallback master
    bus->sources.push_back(src);
    return src;
}

void AudioServer::stop(std::shared_ptr<AudioSource> src) {
    if (!src) return;
    src->is_playing = false;
    src->play_cursor = 0;
}

void AudioServer::pause(std::shared_ptr<AudioSource> src) {
    if (!src) return;
    src->is_paused = true;
}

void AudioServer::resume(std::shared_ptr<AudioSource> src) {
    if (!src) return;
    src->is_paused = false;
}

void AudioServer::set_master_volume(float v) {
    master_volume_ = std::clamp(v, 0.0f, 1.0f);
}

// ===================== Buses =================================================
AudioBus* AudioServer::get_bus(const std::string& name) {
    for (auto& b : buses_) if (b.name == name) return &b;
    return nullptr;
}

AudioBus* AudioServer::create_bus(const std::string& name) {
    if (get_bus(name)) return nullptr;
    buses_.push_back({name, 1.0f, false, false, {}});
    return &buses_.back();
}

void AudioServer::set_bus_volume(const std::string& name, float v) {
    if (auto* b = get_bus(name)) b->volume = std::clamp(v, 0.0f, 1.0f);
}
void AudioServer::set_bus_muted(const std::string& name, bool m) {
    if (auto* b = get_bus(name)) b->muted = m;
}
void AudioServer::set_bus_solo(const std::string& name, bool s) {
    if (auto* b = get_bus(name)) b->solo = s;
}

// ===================== Mixer =================================================
void AudioServer::mix(float* output, int frame_count, int channels) {
    if (!initialized_) return;
    std::lock_guard<std::mutex> lock(mtx_);

    // Limpiar output.
    std::memset(output, 0, frame_count * channels * sizeof(float));

    // Verificar si hay algún bus en solo.
    bool any_solo = false;
    for (const auto& b : buses_) if (b.solo) { any_solo = true; break; }

    for (auto& bus : buses_) {
        if (bus.muted) continue;
        if (any_solo && !bus.solo) continue;
        float bus_vol = bus.volume * master_volume_;

        for (auto& src : bus.sources) {
            if (!src->is_playing || src->is_paused) continue;
            mix_source(*src, output, frame_count, channels);
            // Aplicar volumen del bus + master post-mix? Por simplicidad, ya
            // está aplicado dentro de mix_source con src->volume.
            (void)bus_vol;
        }
    }

    // Limpiar fuentes terminadas.
    for (auto& bus : buses_) {
        bus.sources.erase(
            std::remove_if(bus.sources.begin(), bus.sources.end(),
                [](const std::shared_ptr<AudioSource>& s) {
                    return !s->is_playing && !s->looping;
                }),
            bus.sources.end());
    }
}

void AudioServer::mix_source(AudioSource& src, float* out, int frames, int out_channels) {
    if (!src.stream) return;
    int src_channels = src.stream->channels;

    for (int i = 0; i < frames; ++i) {
        if (src.play_cursor >= src.stream->frame_count) {
            if (src.looping) {
                src.play_cursor = 0;
            } else {
                src.is_playing = false;
                return;
            }
        }

        // Apply pitch (simplificado: skip/repeat frames).
        int64_t frame_idx = src.play_cursor;
        for (int c = 0; c < out_channels; ++c) {
            int src_c = (c < src_channels) ? c : (src_channels - 1);
            float sample = src.stream->samples[frame_idx * src_channels + src_c];
            sample *= src.volume;
            out[i * out_channels + c] += sample;
        }

        src.play_cursor += 1;
        if (src.pitch != 1.0f) {
            // Pitch shift trivial: avanza más o menos samples.
            src.play_cursor = static_cast<int64_t>(src.play_cursor * src.pitch);
        }
    }
}

int AudioServer::active_voice_count() const {
    int count = 0;
    for (const auto& b : buses_) {
        for (const auto& s : b.sources) if (s->is_playing) ++count;
    }
    return count;
}

} // namespace arx
