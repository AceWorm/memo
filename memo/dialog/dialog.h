/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QTimer>
#include <QSystemTrayIcon>
#include "settings.h"
#include "save_memo.h"
#include "help.h"
#include "query_memo.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void closeEvent( QCloseEvent *event );
    void keyPressEvent( QKeyEvent *event  );

private:
    void initVars();
    void initSystemTray();
    void initShortcut();
    void initConfigure();
    void activeGifMovie( const QString &data );

public slots:
    void addAnimation( const QString gifFile );

private slots:
    void topShow();
    void saveMemoData();
    void updateSetting();
    void settingSlot();
    void exitSlot();
    void hideToSystemTrayIcon();
    void subAnimate( int frameNumber );
    void selectedDate( const QString date );
    void refreshTitle( const QString& date = QString() );
    void refreshContent( const QString& contentDate = QString() );
    void activated( QSystemTrayIcon::ActivationReason reason );

    void on_helpPushButton_clicked();
    void on_insertImgPushButton_clicked();
    void on_snapshotPushButton_clicked();
    void on_calendarPushButton_clicked();
    void onCompleteCature( QPixmap captureImage );

    void on_textColorPushButton_clicked();
    void on_textBackgroundColorPushButton_clicked();
    void on_boldTextPushButton_clicked();
    void on_italicTextPushButton_clicked();
    void on_underlineTextPushButton_clicked();
    void on_increaseSizePushButton_clicked();
    void on_decreaseSizePushButton_clicked();

private:
    Ui::Dialog *ui;
    bool m_exitFlag;
    bool m_queryFlag;
    bool m_isSysTrayAvailable;
    QString m_titleDate;
    QStringList m_gifFileList;
    QTimer m_saveTimer;
    QHash <QMovie*, QUrl> m_gifUrls;

    HelpInfo *m_helpInfo;
    SaveMemo *m_saveMemo;
    Settings *m_settings;
    QMenu *m_trayIconMenu;
    QSystemTrayIcon *m_trayIcon;
    QueryMemo *m_queryMemo;
    QAction *m_showMainWindow, *m_configAction, *m_exitAction;
};

#endif // DIALOG_H
