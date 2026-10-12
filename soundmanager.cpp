#include "soundmanager.h"

#include <QUrl>

QHash<QString, QSoundEffect*> SoundManager::soundCache;
bool SoundManager::muted = false;

void SoundManager::playSound(const QString &soundName)
{
    if (muted)
        return;

    QSoundEffect *effect = getEffect(soundName);

    if (!effect)
        return;

    // restart repeated sounds cleanly
    if (effect->isPlaying())
        effect->stop();

    effect->play();
}

void SoundManager::setMuted(bool value)
{
    muted = value;
}

bool SoundManager::isMuted()
{
    return muted;
}

QSoundEffect *SoundManager::getEffect(const QString &soundName)
{
    if (soundCache.contains(soundName))
        return soundCache.value(soundName);

    QSoundEffect *effect = new QSoundEffect();

    QString path = QString("qrc:/sounds/%1.wav").arg(soundName);

    effect->setSource(QUrl(path));
    effect->setVolume(0.65f);
    effect->setLoopCount(1);

    soundCache.insert(soundName, effect);

    return effect;
}
