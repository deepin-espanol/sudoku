#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QAudioSink>
#include <QMediaDevices>
#include <QByteArray>
#include <QBuffer>
#include <QVector>

class SoundManager : public QObject
{
    Q_OBJECT

public:
    explicit SoundManager(QObject *parent = nullptr);
    ~SoundManager() override;

    void playPlace();
    void playClear();
    void playError();
    void playVictory();

    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

    void setVolume(qreal volume);
    qreal volume() const { return m_volume; }

private:
    void playTone(double frequency, int durationMs, double volume = 1.0);
    void playChord(const QVector<double> &frequencies, int durationMs, double volume = 1.0);
    void playBuffer(const QByteArray &data);

    QByteArray generateTone(double frequency, int durationMs, double volume = 1.0) const;
    QByteArray generateChord(const QVector<double> &frequencies, int durationMs, double volume = 1.0) const;

    QAudioSink *m_audioSink = nullptr;
    QBuffer *m_buffer = nullptr;
    bool m_muted = false;
    qreal m_volume = 0.5;

    static constexpr int SAMPLE_RATE = 44100;
};

#endif
