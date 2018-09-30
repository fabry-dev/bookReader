#include <QApplication>
#include "qwindow.h"
#include "qscreen.h"
#include "qwidget.h"
#include "slidewindow.h"
#include "qdebug.h"
#include "videoplayer.h"
#include "slidevideo.h"
#include "rfidreader.h"
#include "serialwatcher.h"

#define PATH_DEFAULT (QString)"/home/fred/Dropbox/Taf/Cassiopee/falcon/files/"

#define imgCount 5
#define defaultSpeed (55)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCursor cursor(Qt::BlankCursor);
    a.setOverrideCursor(cursor);
    a.changeOverrideCursor(cursor);


    QString PATH;
    QStringList params = a.arguments();
    if(params.size()>1)
        PATH = params[1];
    else
        PATH=PATH_DEFAULT;


    int speed = defaultSpeed;


    QFile file(PATH+"config.cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"no config file";

    }
    else
    {

        QTextStream in(&file);

        QString  line;
        QString paramName,paramValue;
        QStringList params;

        while(!in.atEnd()) {
            line = in.readLine();
            line = (line.split("#"))[0];
            params = line.split("=");
            if(params.size()>=2)
            {
                paramName = params[0];
                paramValue = params[1];

                if (paramName=="SPEED")
                {
                    bool test;
                    speed = paramValue.toInt(&test);
                    if(!test)
                        speed = defaultSpeed;
                }
                else
                    qDebug()<<paramName<<" - "<<paramValue;
            }
        }
        file.close();

    }









    //new RFIDReader(NULL);
    serialWatcher * serialwatch = new serialWatcher(NULL);

    //

    QWidget w1;
    QWidget w2;
    QWidget w0;


    w1.setAttribute(Qt::WA_DeleteOnClose);
    w0.setAttribute(Qt::WA_DeleteOnClose);
    w2.setAttribute(Qt::WA_DeleteOnClose);






    QScreen* screen0 = a.screens().at(0);
    QScreen* screen1 = a.screens().at(1);
    QScreen* screen2 = a.screens().at(2);


    w0.setGeometry(screen0->geometry().x(),screen0->geometry().y(),1920*2,1079);
    w1.setGeometry(screen1->geometry().x(),screen1->geometry().y(),1920,1080);
    w2.setGeometry(screen2->geometry().x(),screen2->geometry().y(),1920,1080);


    QList <QWidget*> widgetList;

    widgetList.append(&w1);
    widgetList.append(&w0);
    widgetList.append(&w2);
    w0.showFullScreen();
    w1.showFullScreen();
    w2.showFullScreen();

    std::vector<int> x0s;
    QStringList names;
    QImage buf;
    int totalWidth = 0;


    for(int i =0;i<imgCount;i++)
    {
        QString filename = PATH+"img"+QString::number(i)+".jpg";
        buf.load(filename);
        buf = buf.scaledToHeight(1080);
        x0s.push_back(totalWidth);
        qDebug()<<totalWidth;
        totalWidth+= buf.width();
        names.push_back((QString)"img"+QString::number(i)+".jpg");
    }

    int videoWidth = 1920;
    int videoPos = totalWidth;
    QString videoName = PATH+"video0.mp4";
    totalWidth+=videoWidth;




    if(totalWidth<widgetList.size()*1920)
        totalWidth = widgetList.size()*1920;

    for(int i =0;i<imgCount;i++)
    {
        slideWindow *sw =   new slideWindow(NULL,PATH,widgetList,x0s.at(i),totalWidth,names.at(i),speed,i);
        a.connect(serialwatch,SIGNAL(goBackward()),sw,SLOT(goBackward()));
        a.connect(serialwatch,SIGNAL(goForward()),sw,SLOT(goForward()));



    }
    slidevideo *sv = new slidevideo(NULL,PATH,widgetList,videoPos,videoWidth,totalWidth,videoName,speed);
    a.connect(serialwatch,SIGNAL(goForward()),sv,SLOT(goForward()));
    a.connect(serialwatch,SIGNAL(goBackward()),sv,SLOT(goBackward()));


    return a.exec();
}
