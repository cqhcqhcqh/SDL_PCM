#include "playthread.h"
#include <QDebug>
extern "C" {
#include <SDL2/SDL.h>
}
#include <QFile>
#ifdef Q_OS_MAC
#define FILENAME "/Users/caitou/Desktop/out.pcm"
#elif Q_OS_Win32
#define FILENAME "/Users/caitou/Desktop/out.pcm"
#else
#define FILENAME "/Users/caitou/Desktop/out.pcm"
#endif
#define SAMPLES 1024
#define CHANNELS 2
#define SAMPLE_SIZE 16
#define BUFFER_SIZE (CHANNELS * SAMPLE_SIZE * SAMPLES / 8)

char* buffer_data;
int buffer_len;
void audioCallback(void *userdata,
                   Uint8 * stream,
                       int len) {

    SDL_memset(stream, 0, len);

    if (buffer_len <= 0) return;

    len = buffer_len > len ? len : buffer_len;
    qDebug() << "begin consume buffer_len: " << buffer_len;
    SDL_MixAudio(stream, (Uint8 *) buffer_data, len, SDL_MIX_MAXVOLUME);
    buffer_data += len;
    buffer_len -= len;
    qDebug() << "after consume buffer_len: " << buffer_len;
}

PlayThread::PlayThread(QObject *parent)
    : QThread{parent}
{
    connect(this, &PlayThread::finished, this, &PlayThread::deleteLater);
}

PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "has been destoryed";
}

void PlayThread::run() {
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "SDL_Init error" << SDL_GetError();
        return;
    }

    SDL_AudioSpec spec;
    spec.freq = 41000;
    spec.channels = CHANNELS;
    spec.format = AUDIO_S16SYS;
    spec.samples = SAMPLES;
    spec.callback = audioCallback;

    int res = SDL_OpenAudio(&spec, nullptr);
    if (res != 0) {
        qDebug() << "SDL_OpenAudio" << SDL_GetError();
        SDL_Quit();
        return;
    }

    QFile file(FILENAME);
    res = file.open(QFile::ReadOnly);
    if (res == 0) {
       qDebug() << "QFile open error code" << res;
       SDL_CloseAudio();
       SDL_Quit();
    }

    SDL_PauseAudio(0);

    char data[BUFFER_SIZE];

    while (!isInterruptionRequested() && (buffer_len = file.read(data, BUFFER_SIZE))) {
        qDebug() << "read buffer_len: " << buffer_len;
        buffer_data = data;
        while (buffer_len > 0) {
            SDL_Delay(1);
        }
    }

    file.close();

    SDL_CloseAudio();
    SDL_Quit();
}
