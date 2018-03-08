#ifndef UPDATESYSTEM_H
#define UPDATESYSTEM_H

#include <QThread>
#include "singleton.h"
#include "mainctrl.h"
#include "db.h"
#include "exceptioncomm.h"
#include "servercomm.h"
#include "httpupdate.h"
#include <QObject>
#include <QTime>
#include <QDateTime>
#include <QDebug>

#define UPDATE_SLEEP_TIME  25*60

class CUpdateSystemThrd : public QThread
{
    Q_OBJECT
public:

    void init()
    {
        start();
    }

signals:


private:
    CUpdateSystemThrd(){}
    ~CUpdateSystemThrd(){}


    void run()
    {

        qsrand(time(0));

        while(1)
        {
            sleep(UPDATE_SLEEP_TIME);

            if (SCMainCtrl::instance()->curStatus() == 1)
            {
                QTime time = QTime::currentTime();
                int hour = time.hour();
                int updatetime;
                if(hour == 2)

                {
                    updatetime = qrand()%30;
                    qDebug() << "start update program" << updatetime;

                    sleep(updatetime*60);

                    updateAppSoftWare();
                }
            }
        }

    }

    void  updateAppSoftWare()
    {
        qDebug() << "--info--: update thread proccess ...";

        QString name ="Qt.zip";
        QDateTime serverTime;
        QString version = SCDatBase::instance()->getAppVersion();

        isConfirmationResp respone;
        SCExceptioncomm::instance()->isConfirmation(name,version,&respone);

        QString date = respone.systemDate();
        serverTime = QDateTime::fromString(date,Qt::ISODate);

        if(!date.isEmpty())
        {
            if(serverTime != QDateTime::currentDateTime())
            {
                date = "\"" + date + "\"";
                QString temp = QString("date -s %1").arg(date);
                QByteArray clockData = temp.toLatin1();
                system(clockData.data());

                system("hwclock -u -w");
                system("sed -i 's/unzip file/unzip -o file/g' update.sh");

              //  qDebug() << "system clockData";

            }
        }

        if(respone.success() == "true")
        {
            if(SCExceptioncomm::instance()->download(name,respone.md5No()))
            {
                SCDatBase::instance()->setAppVersion(respone.creatTime());

               // qDebug() << "system update";


                system("sh update.sh &");
            }
            else
            {
                system("rm -rf ./file/*");
            }
        }

    }

    DECLARE_SINGLETON_CLASS(CUpdateSystemThrd)
};

typedef Singleton<CUpdateSystemThrd> SCUpdateSystemThrd;

#endif // UPDATESYSTEM_H
