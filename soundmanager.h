#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QString>
#include <QHash>
#include <QSoundEffect>

// loads and plays sound effects
class SoundManager
{
public:
    static void playSound(const QString &soundName);
    static void setMuted(bool value);
    static bool isMuted();

private:
    static QSoundEffect *getEffect(const QString &soundName);

    static QHash<QString, QSoundEffect*> soundCache;
    static bool muted;
};

#endif // SOUNDMANAGER_H
