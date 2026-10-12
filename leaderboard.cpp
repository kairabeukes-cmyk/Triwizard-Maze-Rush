#include "leaderboard.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>

ScoreEntry::ScoreEntry()
    : playerName("Player"),
      timeSeconds(0.0f)
{
}

ScoreEntry::ScoreEntry(const QString &name, float time)
    : playerName(name),
      timeSeconds(time)
{
}

Leaderboard::Leaderboard()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (folder.isEmpty())
        folder = ".";

    QDir dir(folder);

    if (!dir.exists())
        dir.mkpath(".");

    filePath = folder + "/scores.txt";

    load();
}

Leaderboard::Leaderboard(const QString &filePath)
    : filePath(filePath)
{
    load();
}

void Leaderboard::addScore(const QString &playerName, float timeSeconds)
{
    scores.append(ScoreEntry(playerName, timeSeconds));
    sortScores();
    save();
}

QVector<ScoreEntry> Leaderboard::getScores() const
{
    return scores;
}

QVector<ScoreEntry> Leaderboard::getTopScores(int count) const
{
    QVector<ScoreEntry> topScores;

    int limit = count;

    if (limit > scores.size())
        limit = scores.size();

    for (int i = 0; i < limit; ++i)
        topScores.append(scores[i]);

    return topScores;
}

void Leaderboard::load()
{
    scores.clear();

    QFile file(filePath);

    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        QStringList parts = line.split(",");

        if (parts.size() != 2)
            continue;

        QString name = parts[0].trimmed();
        bool ok = false;
        float time = parts[1].trimmed().toFloat(&ok);

        if (ok)
            scores.append(ScoreEntry(name, time));
    }

    file.close();

    sortScores();
}

void Leaderboard::save() const
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    for (int i = 0; i < scores.size(); ++i)
    {
        out << scores[i].playerName << ","
            << QString::number(scores[i].timeSeconds, 'f', 2)
            << "\n";
    }

    file.close();
}

QString Leaderboard::formatTime(float seconds) const
{
    int totalCentiseconds = static_cast<int>(seconds * 100.0f);

    int minutes = totalCentiseconds / 6000;
    int remainingCentiseconds = totalCentiseconds % 6000;

    int secs = remainingCentiseconds / 100;
    int centiseconds = remainingCentiseconds % 100;

    return QString("%1:%2.%3")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'))
            .arg(centiseconds, 2, 10, QChar('0'));
}

void Leaderboard::sortScores()
{
    std::sort(scores.begin(), scores.end(),
              [](const ScoreEntry &a, const ScoreEntry &b)
              {
                  return a.timeSeconds < b.timeSeconds;
              });
}
