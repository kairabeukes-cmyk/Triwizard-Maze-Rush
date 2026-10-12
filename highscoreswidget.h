#ifndef HIGHSCORESWIDGET_H
#define HIGHSCORESWIDGET_H

#include <QWidget>
#include <QPixmap>

#include "leaderboard.h"

class QPushButton;
class QListWidget;

// high score screen
class HighScoresWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HighScoresWidget(QWidget *parent = nullptr);

    void refreshScores();

signals:
    void backClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QListWidget *scoresList;
    QPushButton *backButton;

    Leaderboard leaderboard;
    QPixmap backgroundImage;
};

#endif // HIGHSCORESWIDGET_H
