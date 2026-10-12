#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <QString>
#include <QVector>

// one saved score entry
struct ScoreEntry
{
    QString playerName;
    float timeSeconds;

    ScoreEntry();
    ScoreEntry(const QString &name, float time);
};

// loads, saves, sorts, and formats high scores
class Leaderboard
{
public:
    Leaderboard();
    explicit Leaderboard(const QString &filePath);

    void addScore(const QString &playerName, float timeSeconds);
    QVector<ScoreEntry> getScores() const;
    QVector<ScoreEntry> getTopScores(int count) const;

    void load();
    void save() const;

    QString formatTime(float seconds) const;

private:
    QVector<ScoreEntry> scores;
    QString filePath;

    void sortScores();
};

#endif // LEADERBOARD_H
