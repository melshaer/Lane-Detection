//

#include <QtGui>
#include <QThread>

#include "main.h"
#include "mainwin.h"

extern "C" {
int mymain(int);
}

class MyThread : public QThread
{
public:
	void run();
};

void MyThread::run() {
	mymain(1);
}


MainWindow::MainWindow()
{
    int i,j,k;

#ifndef BENCHTEST
    system("killall -i gigecamera");
#endif

    QWidget *mainwidget= new QWidget;
    setCentralWidget(mainwidget);

    QWidget *asd = new QWidget;
    asd->setFixedSize(900,500);
//    asd->setMinimumSize(900,500);

    QGridLayout *grid= new QGridLayout;
    grid->addWidget(asd, 0, 1);
    QVBoxLayout *leftBox= new QVBoxLayout;

    modeLBL= new QLabel("Mode", asd);
    modeLBL->setFont(QFont("Times", 14, QFont::Normal));
    modeLBL->setGeometry(20, 30, 200, 40);
    logSuffixLBL= new QLabel("LogSuffix", asd);
    logSuffixLBL->setFont(QFont("Times", 14, QFont::Normal));
    logSuffixLBL->setGeometry(250,  30, 400, 40);
    frameCntLbl= new QLabel("FrameCnt 0", asd);
    frameCntLbl->setFont(QFont("Times", 12, QFont::Normal));
    frameCntLbl->setGeometry(120, 80, 130, 20);
    laneOffsetLbl= new QLabel("LaneOffset 0", asd);
    laneOffsetLbl->setFont(QFont("Times", 12, QFont::Normal));
    laneOffsetLbl->setGeometry(300, 80, 130, 20);
    frameRateLbl= new QLabel("FrameRate 0.0", asd);
    frameRateLbl->setFont(QFont("Times", 12, QFont::Normal));
    frameRateLbl->setGeometry(120, 120, 130, 20);

#define LBLWIDTH 80
const char *varnames[]= {"GPSTs", "PosSecs","SolnStat","PosType", 
   "#Obs", "Latitude", "Longitude","HorzSpd", "TrkGnd", "LatStd",
   "CanTs", "AccelPos", "BrakePres", "StrWhl", "Speed1", "EngRPM", "BrkPdl",
    "PRNDL", "TurnSig"};
const int varxpos[]={0,90,180,260,370,420,520,630,720,810,900};

    for (i=0; i<10; i++) {
       varLbl[i]= new QLabel(varnames[i], asd);
       varLbl[i]->setFont(QFont("Times", 14, QFont::Normal));
       varLbl[i]->setGeometry (20+varxpos[i], 170, varxpos[i+1]-varxpos[i], 20);
       varValue[i]= new QLabel("0.0", asd);
       varValue[i]->setFont(QFont("Times", 14, QFont::Normal));
       varValue[i]->setGeometry(20+varxpos[i], 200, varxpos[i+1]-varxpos[i],20);
    }
    varValue[0]->setText("00000.00");
    varValue[1]->setText("654321.00");
    for (i=10; i<19; i++) {
       varLbl[i]= new QLabel(varnames[i], asd);
       varLbl[i]->setFont(QFont("Times", 14, QFont::Normal));
       varLbl[i]->setGeometry (20+i*(LBLWIDTH+10)-10*(LBLWIDTH+10), 250, 
                               LBLWIDTH, 20);
       varValue[i]= new QLabel("0.0", asd);
       varValue[i]->setFont(QFont("Times", 14, QFont::Normal));
       varValue[i]->setGeometry(20+i*(LBLWIDTH+10)-10*(LBLWIDTH+10), 280, 
                                LBLWIDTH, 20);
    }
#if 0
    for (i=20; i<22; i++) {
       varLbl[i]= new QLabel(varnames[i], asd);
       varLbl[i]->setFont(QFont("Times", 14, QFont::Normal));
       varLbl[i]->setGeometry (20+i*(LBLWIDTH+10)-20*(LBLWIDTH+10), 330, 
                               LBLWIDTH, 20);
       varValue[i]= new QLabel("0.0", asd);
       varValue[i]->setFont(QFont("Times", 14, QFont::Normal));
       varValue[i]->setGeometry(20+i*(LBLWIDTH+10)-20*(LBLWIDTH+10), 360, 
                                LBLWIDTH, 20);
    }
#endif

    manStartPB = new QPushButton( "&ManStart" );
    manStartPB->setMaximumWidth(110);
    manStartPB->setFont( QFont( "Times", 14, QFont::Bold ) );
    manStartPB->setToolTip(tr("Manually Start a Data Collection"));
    connect( manStartPB, SIGNAL(clicked()), this, SLOT(manStart()) );
    manStopPB = new QPushButton( "&Stop" );
    manStopPB->setMaximumWidth(110);
    manStopPB->setFont( QFont( "Times", 14, QFont::Bold ) );
    manStopPB->setToolTip(tr("Stop Data Collection"));
    connect( manStopPB, SIGNAL(clicked()), this, SLOT(manStop()) );
    quitPB = new QPushButton( "&Quit" );
    quitPB->setMaximumWidth(110);
    quitPB->setFont( QFont( "Times", 14, QFont::Bold ) );
    quitPB->setToolTip(tr("Quit Program"));
    connect( quitPB, SIGNAL(clicked()), this, SLOT(close()) );

    leftBox->addWidget(manStartPB);
    leftBox->addWidget(manStopPB);
    leftBox->addSpacing(20);
    leftBox->addWidget(quitPB);
    grid->addLayout(leftBox, 0, 0);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 0);
    mainwidget->setLayout(grid);

    createActions();
    createMenus();

    setWindowTitle(tr("QT SDL Image Test"));

    updateTimer = new QTimer( this );
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()) );
    updateTimer->start( 200 );

    MyThread *thread1= new MyThread;
    thread1->start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        sdl_thread_die= 1;
        sleep(2);
#ifndef BENCHTEST
        system("killall -9 gigecamera");
#endif
        event->accept();
        //event->ignore();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Lane Tracker Test"),
      tr("<p>The <b>Lane Tracker</b> test software collected image sequences"
        " from a Point Grey GigE camera, logs if selected, and executes the"
        " prototype.</p>"
	" Oct 2014.</p>"
	"</p>Start/stop is manual at present.</p>"
         "<p> Author: Keith A. Redmill, The Ohio State University, 2014</p>"
         "<p> No Warrantees.  Use at your own risk.  Your mileage may vary.</p>"));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct= new QAction(tr("&Save..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT (save()));

    printAct = new QAction(tr("&Print..."), this);
//    connect(printAct, SIGNAL(triggered()), worldCFG, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    configAct = new QAction(tr("&Config"), this);
//    connect(configAct, SIGNAL(triggered()), worldCFG, SLOT(exposeConfig()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(configAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::open()
{   
}   
    
void MainWindow::save()
{
}

void MainWindow::manStart()
{
  Gacqframe= 0;
}

void MainWindow::manStop() {
  Gacqframe= -1;
}

void MainWindow::update() {
  int i,j,k;
  QString line;
  char line2[40];

  frameCntLbl->setText(line.sprintf("FrameCnt %d", framecount));
  frameRateLbl->setText(line.sprintf("FrameRate %f", framerate));
  laneOffsetLbl->setText(line.sprintf("LaneOffset %d", Goffsetpixels));
  modeLBL->setText(line.sprintf("MODE: tbd"));
  logSuffixLBL->setText(line.sprintf("LOGNAME: %s", "tbd"));

  varValue[0]->setText(line.sprintf("%8.2f", vehshm->gps1.timestamp));
  varValue[1]->setText(line.sprintf("%8.1f", vehshm->gps1.posseconds));
  varValue[2]->setText(line.sprintf("%d", vehshm->gps1.solnstatus));
  varValue[3]->setText(line.sprintf("%d", vehshm->gps1.postype));
  switch (vehshm->gps1.postype) {
    case 0: 
      varValue[3]->setText("0 None");
      break;
    case 16:
      varValue[3]->setText("16 Single");
      break;
    case 17:
      varValue[3]->setText("17 PsrDiff");
      break;
    case 18:
      varValue[3]->setText("18 Waas");
      break;
    case 20:
      varValue[3]->setText("20 OmniVBS");
      break;
    case 32:
      varValue[3]->setText("32 Float");
      break;
    case 48:
      varValue[3]->setText("48 L1Int");
      break;
    case 49:
      varValue[3]->setText("49 WideInt");
      break;
    case 50:
      varValue[3]->setText("50 NarInt");
      break;
    case 64:
      varValue[3]->setText("64 OmniHP");
      break;
  }
  varValue[4]->setText(line.sprintf("%d", vehshm->gps1.numobs));
  varValue[5]->setText(line.sprintf("%9.6f", vehshm->gps1.lat));
  varValue[6]->setText(line.sprintf("%10.6f", vehshm->gps1.lng));
  varValue[7]->setText(line.sprintf("%5.2f", vehshm->gps1.horzspeed));
  varValue[8]->setText(line.sprintf("%6.2f", vehshm->gps1.trkgnd));
  varValue[9]->setText(line.sprintf("%6.3f", vehshm->gps1.latstd));
  varValue[10]->setText(line.sprintf("%8.2f", vehshm->cants));
  varValue[11]->setText(line.sprintf("%6.2f", vehshm->accelpos));
if (vehshm->brakepressure>-1.0 && vehshm->brakepressure<1000.0)
  varValue[12]->setText(line.sprintf("%7.2f", vehshm->brakepressure));
  varValue[13]->setText(line.sprintf("%6.2f", vehshm->steeringwheel));
  varValue[14]->setText(line.sprintf("%5.2f", vehshm->speed1));
  varValue[15]->setText(line.sprintf("%7.2f", vehshm->enginerpm));
  varValue[16]->setText(line.sprintf("%d", vehshm->brakepdl));
  varValue[17]->setText(line.sprintf("%x", vehshm->prndl));
  varValue[18]->setText(line.sprintf("%d", vehshm->turnsignal));
}
