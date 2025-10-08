#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <miniaudio.h>
#include <string>
#include <iostream>
#include <map>

using namespace std;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    bool loadMusic(const string& filePath);
    void playMusic(bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume); // 0.0 a 1.0
    bool isMusicPlaying();

    // Efeitos sonoros
    bool loadSound(const string& name, const string& filePath);
    void playSound(const string& name, float volume = 1.0f);

    void cleanup();

private:
    ma_engine engine;
    ma_sound music;
    map<string, ma_sound*> sounds; // Mapa de efeitos sonoros
    bool initialized;
    bool musicLoaded;
};

#endif // AUDIOMANAGER_H
