#include "highscoreswidget.h"

#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>

HighScoresWidget::HighScoresWidget(QWidget *parent)
    : QWidget(parent),
      scoresList(nullptr),
      backButton(nullptr),
      backgroundImage(":/sprites/highscores_background.png")
{
    setMinimumSize(1000, 700);

    setStyleSheet(
        "QWidget {"
        " color: white;"
        "}"
        "QListWidget {"
        " background-color: rgba(15, 15, 25, 175);"
        " color: white;"
        " border: 2px solid rgba(240, 210, 120, 190);"
        " border-radius: 14px;"
        " padding: 12px;"
        " font-size: 17px;"
        " font-weight: bold;"
        "}"
        "QListWidget::item {"
        " padding: 8px;"
        "}"
        "QPushButton {"
        " background-color: rgba(25, 25, 35, 180);"
        " color: white;"
        " border: 2px solid rgb(230, 200, 110);"
        " border-radius: 12px;"
        " padding: 12px;"
        " font-size: 15px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(80, 65, 115, 220);"
        " border: 2px solid rgb(255, 230, 150);"
        "}"
        "QPushButton:pressed {"
        " background-color: rgba(35, 25, 55, 230);"
        "}"
    );

    scoresList = new QListWidget(this);
    scoresList->setMinimumSize(470, 310);
    scoresList->setMaximumWidth(560);

    backButton = new QPushButton("Back to Main Menu", this);
    backButton->setMinimumWidth(250);
    backButton->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // leave space for the background title
    mainLayout->addStretch();
    mainLayout->addSpacing(120);
    mainLayout->addWidget(scoresList, 0, Qt::AlignCenter);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    connect(backButton, &QPushButton::clicked,
            this, &HighScoresWidget::backClicked);

    refreshScores();
}

void HighScoresWidget::refreshScores()
{
    scoresList->clear();

    leaderboard.load();

    QVector<ScoreEntry> scores = leaderboard.getTopScores(10);

    if (scores.isEmpty())
    {
        scoresList->addItem("No scores saved yet.");
        return;
    }

    for (int i = 0; i < scores.size(); ++i)
    {
        QString line = QString("%1. %2 - %3")
                           .arg(i + 1)
                           .arg(scores[i].playerName)
                           .arg(leaderboard.formatTime(scores[i].timeSeconds));

        scoresList->addItem(line);
    }
}

void HighScoresWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (!backgroundImage.isNull())
    {
        painter.drawPixmap(rect(), backgroundImage);
    }
    else
    {
        painter.fillRect(rect(), QColor(25, 25, 35));

        painter.setPen(QColor(255, 230, 150));
        painter.setFont(QFont("Georgia", 34, QFont::Bold));
        painter.drawText(QRectF(0, 60, width(), 80),
                         Qt::AlignCenter,
                         "High Scores");
    }

    // light overlay for readability
    painter.fillRect(rect(), QColor(0, 0, 0, 25));
}
