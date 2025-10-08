#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"

AudioManager::AudioManager() : initialized(false), musicLoaded(false) {
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        cerr << "Erro ao inicializar o motor de áudio." << endl;
        return false;
    }

    initialized = true;
    cout << "Sistema de áudio inicializado com sucesso!" << endl;
    return true;
}

bool AudioManager::loadMusic(const string& filePath) {
    if (!initialized) {
        cerr << "Sistema de áudio não foi inicializado!" << endl;
        return false;
    }

    // Se já tem música carregada, libera primeiro
    if (musicLoaded) {
        ma_sound_uninit(&music);
        musicLoaded = false;
    }

    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(),
        MA_SOUND_FLAG_STREAM, NULL, NULL, &music);

    if (result != MA_SUCCESS) {
        cerr << "Erro ao carregar música: " << filePath << endl;
        return false;
    }

    musicLoaded = true;
    cout << "Música carregada: " << filePath << endl;
    return true;
}

void AudioManager::playMusic(bool loop) {
    if (!musicLoaded) {
        cerr << "Nenhuma música carregada!" << endl;
        return;
    }

    ma_sound_set_looping(&music, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&music);
    cout << "Música tocando " << (loop ? "(loop ativado)" : "(sem loop)") << endl;
}

void AudioManager::stopMusic() {
    if (musicLoaded) {
        ma_sound_stop(&music);
        ma_sound_seek_to_pcm_frame(&music, 0); // Volta ao início
    }
}

void AudioManager::pauseMusic() {
    if (musicLoaded) {
        ma_sound_stop(&music);
    }
}

void AudioManager::resumeMusic() {
    if (musicLoaded) {
        ma_sound_start(&music);
    }
}

void AudioManager::setMusicVolume(float volume) {
    if (musicLoaded) {
        // Limita o volume entre 0.0 e 1.0
        if (volume < 0.0f) volume = 0.0f;
        if (volume > 1.0f) volume = 1.0f;
        ma_sound_set_volume(&music, volume);
    }
}

bool AudioManager::isMusicPlaying() {
    if (!musicLoaded) return false;
    return ma_sound_is_playing(&music);
}

// ===== EFEITOS SONOROS =====

bool AudioManager::loadSound(const string& name, const string& filePath) {
    if (!initialized) {
        cerr << "Sistema de áudio não foi inicializado!" << endl;
        return false;
    }

    // Se já existe um som com esse nome, libera primeiro
    if (sounds.find(name) != sounds.end()) {
        ma_sound_uninit(sounds[name]);
        delete sounds[name];
        sounds.erase(name);
    }

    // Criar novo som
    ma_sound* sound = new ma_sound();
    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(),
        MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, sound);

    if (result != MA_SUCCESS) {
        cerr << "Erro ao carregar som '" << name << "': " << filePath << endl;
        delete sound;
        return false;
    }

    sounds[name] = sound;
    cout << "Som carregado: " << name << " (" << filePath << ")" << endl;
    return true;
}

void AudioManager::playSound(const string& name, float volume) {
    if (!initialized) return;

    // Verificar se o som existe
    if (sounds.find(name) == sounds.end()) {
        cerr << "Som '" << name << "' não foi carregado!" << endl;
        return;
    }

    ma_sound* sound = sounds[name];

    // Configurar volume
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    ma_sound_set_volume(sound, volume);

    // Parar o som se já estiver tocando e reiniciar
    if (ma_sound_is_playing(sound)) {
        ma_sound_stop(sound);
    }
    ma_sound_seek_to_pcm_frame(sound, 0); // Volta ao início

    // Tocar o som
    ma_sound_start(sound);
}

void AudioManager::cleanup() {
    // Limpar efeitos sonoros
    for (auto& pair : sounds) {
        ma_sound_uninit(pair.second);
        delete pair.second;
    }
    sounds.clear();

    // Limpar música
    if (musicLoaded) {
        ma_sound_uninit(&music);
        musicLoaded = false;
    }

    if (initialized) {
        ma_engine_uninit(&engine);
        initialized = false;
    }
}
