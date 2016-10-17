//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

//class WorldCFG;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void update();
    void open();
    void save(); 
    void about();
    void manStart();
    void manStop();

private:
    void createActions();
    void createMenus();

    QString fileName;
    QTimer *updateTimer;

    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;

    QPushButton *manStartPB, *manStopPB, *quitPB;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *configAct;
    QAction *printAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QLabel *modeLBL;
    QLabel *logSuffixLBL;
    QLabel *frameCntLbl;
    QLabel *frameRateLbl;
    QLabel *laneOffsetLbl;
    QLabel *varLbl[30];
    QLabel *varValue[30];
};

#endif
