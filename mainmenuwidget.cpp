#include "mainmenuwidget.h"

#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFont>

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent),
      backgroundImage(":/sprites/menu_background.png"),
      singlePlayerButton(nullptr),
      hostButton(nullptr),
      joinButton(nullptr),
      highScoresButton(nullptr),
      exitButton(nullptr),
      singlePlayerDesignRect(370, 260, 260, 55),
      hostDesignRect(370, 325, 260, 55),
      joinDesignRect(370, 389, 260, 55),
      highScoresDesignRect(370, 452, 260, 55),
      exitDesignRect(370, 515, 260, 55)
{
    setMinimumSize(1000, 700);

    singlePlayerButton = createInvisibleButton("Single Player");
    hostButton = createInvisibleButton("Host Multiplayer");
    joinButton = createInvisibleButton("Join Multiplayer");
    highScoresButton = createInvisibleButton("High Scores");
    exitButton = createInvisibleButton("Exit");

    positionButtons();

    connect(singlePlayerButton, &QPushButton::clicked,
            this, &MainMenuWidget::singlePlayerClicked);

    connect(hostButton, &QPushButton::clicked,
            this, &MainMenuWidget::hostMultiplayerClicked);

    connect(joinButton, &QPushButton::clicked,
            this, &MainMenuWidget::joinMultiplayerClicked);

    connect(highScoresButton, &QPushButton::clicked,
            this, &MainMenuWidget::highScoresClicked);

    connect(exitButton, &QPushButton::clicked,
            this, &MainMenuWidget::exitClicked);
}

QPushButton *MainMenuWidget::createInvisibleButton(const QString &name)
{
    QPushButton *button = new QPushButton(this);

    button->setText("");
    button->setToolTip(name);
    button->setAccessibleName(name);
    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(
        "QPushButton {"
        " background-color: rgba(0, 0, 0, 0);"
        " border: none;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(255, 255, 255, 25);"
        " border: 1px solid rgba(255, 230, 150, 90);"
        "}"
        "QPushButton:pressed {"
        " background-color: rgba(255, 230, 150, 45);"
        "}"
    );

    return button;
}

void MainMenuWidget::positionButtons()
{
    if (!singlePlayerButton)
        return;

    singlePlayerButton->setGeometry(scaledRect(singlePlayerDesignRect));
    hostButton->setGeometry(scaledRect(hostDesignRect));
    joinButton->setGeometry(scaledRect(joinDesignRect));
    highScoresButton->setGeometry(scaledRect(highScoresDesignRect));
    exitButton->setGeometry(scaledRect(exitDesignRect));
}

QRect MainMenuWidget::scaledRect(const QRectF &designRect) const
{
    const double designWidth = 1000.0;
    const double designHeight = 700.0;

    double scaleX = width() / designWidth;
    double scaleY = height() / designHeight;

    return QRect(static_cast<int>(designRect.x() * scaleX),
                 static_cast<int>(designRect.y() * scaleY),
                 static_cast<int>(designRect.width() * scaleX),
                 static_cast<int>(designRect.height() * scaleY));
}

void MainMenuWidget::paintEvent(QPaintEvent *event)
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
        painter.drawText(rect(), Qt::AlignCenter, "Triwizard Maze Rush");
    }

    // optional screen tint
    painter.fillRect(rect(), QColor(0, 0, 0, 0));
}

void MainMenuWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    positionButtons();
}
