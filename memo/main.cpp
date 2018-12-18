/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "dialog.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <fcntl.h>
#define APP_MUTEX_FILE_NAME "/tmp/.arm_app_qt_application_.locker"
#endif

bool isRunning()
{
#ifdef Q_OS_WIN32
    HANDLE mutex = CreateMutex( NULL, FALSE, L"_memo_prog_mutex_by_HS" );

    if ( ERROR_ALREADY_EXISTS == GetLastError() )
    {
        CloseHandle( mutex );
        mutex = NULL;

        return true;
    }
#else
    int fd = -1;
    int flock = -1;

    fd = open( APP_MUTEX_FILE_NAME, O_WRONLY | O_CREAT, 0644 );

    if ( 0 > fd )
    {
        return true;
    }

    flock = lockf( fd, F_TLOCK, 0 );

    if ( 0 > flock )
    {
        return true;
    }
#endif

    return false;
}


int main(int argc, char *argv[])
{
    QApplication a( argc, argv );

    if ( isRunning() )
    {
        QMessageBox::warning( NULL, "警告", "本程序已经运行！", QMessageBox::Ok );

        return 0;
    }

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale( codec );

    Dialog w;
    w.show();

    return a.exec();
}
