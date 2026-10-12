#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QRectF>

class QPushButton;

// main menu screen with background image and invisible buttons
class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);

signals:
    void singlePlayerClicked();
    void hostMultiplayerClicked();
    void joinMultiplayerClicked();
    void highScoresClicked();
    void exitClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *createInvisibleButton(const QString &name);

    void positionButtons();

    QRect scaledRect(const QRectF &designRect) const; // scales button areas to window size

    QPixmap backgroundImage;

    QPushButton *singlePlayerButton;
    QPushButton *hostButton;
    QPushButton *joinButton;
    QPushButton *highScoresButton;
    QPushButton *exitButton;

    QRectF singlePlayerDesignRect;
    QRectF hostDesignRect;
    QRectF joinDesignRect;
    QRectF highScoresDesignRect;
    QRectF exitDesignRect;
};

#endif // MAINMENUWIDGET_H
