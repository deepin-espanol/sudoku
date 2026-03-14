#include "SoundManager.h"

#include <cmath>
#include <QAudioFormat>

SoundManager::SoundManager(QObject *parent)
    : QObject(parent)
{
    QAudioFormat format;
    format.setSampleRate(SAMPLE_RATE);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    auto device = QMediaDevices::defaultAudioOutput();
    if (device.isNull()) return;

    m_audioSink = new QAudioSink(device, format, this);
    m_buffer = new QBuffer(this);
}

SoundManager::~SoundManager()
{
    if (m_audioSink) {
        m_audioSink->stop();
    }
}

void SoundManager::setMuted(bool muted)
{
    m_muted = muted;
}

void SoundManager::setVolume(qreal volume)
{
    m_volume = qBound(0.0, volume, 1.0);
}

void SoundManager::playPlace()
{
    if (m_muted || !m_audioSink) return;
    playTone(523.25, 80, 0.3); // C5
}

void SoundManager::playClear()
{
    if (m_muted || !m_audioSink) return;
    playTone(659.25, 100, 0.4); // E5
}

void SoundManager::playError()
{
    if (m_muted || !m_audioSink) return;
    playChord({200.0, 180.0}, 200, 0.3);
}

void SoundManager::playVictory()
{
    if (m_muted || !m_audioSink) return;
    QVector<double> notes = {523.25, 659.25, 783.99, 1046.5}; // C5 E5 G5 C6
    for (double freq : notes) {
        playTone(freq, 150, 0.5);
    }
}

void SoundManager::playTone(double frequency, int durationMs, double volume)
{
    if (m_muted || !m_audioSink) return;
    QByteArray data = generateTone(frequency, durationMs, volume * m_volume);
    playChord({frequency}, durationMs, volume);
}

void SoundManager::playChord(const QVector<double> &frequencies, int durationMs, double volume)
{
    if (m_muted || !m_audioSink) return;
    QByteArray data = generateChord(frequencies, durationMs, volume * m_volume);

    m_audioSink->stop();
    m_buffer->close();
    m_buffer->setData(data);
    m_buffer->open(QIODevice::ReadOnly);
    m_audioSink->start(m_buffer);
}

QByteArray SoundManager::generateTone(double frequency, int durationMs, double volume) const
{
    int samples = SAMPLE_RATE * durationMs / 1000;
    QByteArray data;
    data.resize(samples * 2);

    int16_t *ptr = reinterpret_cast<int16_t*>(data.data());
    for (int i = 0; i < samples; ++i) {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        double value = sin(2.0 * M_PI * frequency * t);
        double envelope = 1.0 - static_cast<double>(i) / samples;
        ptr[i] = static_cast<int16_t>(value * envelope * volume * 30000);
    }

    return data;
}

QByteArray SoundManager::generateChord(const QVector<double> &frequencies, int durationMs, double volume) const
{
    int samples = SAMPLE_RATE * durationMs / 1000;
    QByteArray data;
    data.resize(samples * 2);

    int16_t *ptr = reinterpret_cast<int16_t*>(data.data());
    for (int i = 0; i < samples; ++i) {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        double value = 0.0;
        for (double freq : frequencies) {
            value += sin(2.0 * M_PI * freq * t);
        }
        value /= frequencies.size();
        double envelope = 1.0 - static_cast<double>(i) / samples;
        ptr[i] = static_cast<int16_t>(value * envelope * volume * 30000);
    }

    return data;
}
