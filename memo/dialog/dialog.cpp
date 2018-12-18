/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "dialog.h"
#include "ui_dialog.h"
#include <QProcess>
#include <QMessageBox>
#include <QDate>
#include <QMovie>
#include <QColorDialog>
#include <QMimeData>
#include <QClipboard>
#include <QFileDialog>
#include "capture_screen.h"
#include "qxtglobalshortcut5/gui/qxtglobalshortcut.h"

#ifdef Q_OS_WIN32
#define DEFAULT_DIRECTORY "c:/"
#else
#define DEFAULT_DIRECTORY "/home/hans/"
#endif

#define DEFAULT_SAVE_INTERVAL ( 1000 * 60 * 5 )

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi( this );
//    setFixedSize( size() );
    setWindowFlags( Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint );

    QIcon icon = QIcon( ":/icon.png" );
    setWindowIcon( icon );

    refreshTitle();
    initConfigure();
    initVars();
    initSystemTray();
    initShortcut();
    hideToSystemTrayIcon();
}


Dialog::~Dialog()
{
    if ( NULL != m_settings )
    {
        delete m_settings;
    }

    if ( NULL != m_saveMemo )
    {
        delete m_saveMemo;
    }

    if ( NULL != m_helpInfo )
    {
        delete m_helpInfo;
    }

    if ( NULL != m_queryMemo )
    {
        delete m_queryMemo;
    }

    if ( !m_gifUrls.isEmpty() )
    {
        QHashIterator <QMovie*, QUrl> it( m_gifUrls );

        while ( it.hasNext() )
        {
            it.next();
            it.key()->stop();
            delete it.key();
        }
    }

    delete ui;
}


void Dialog::closeEvent( QCloseEvent *event )
{
    hideToSystemTrayIcon();

    if ( !m_exitFlag )
    {
        event->ignore();
    }
    else
    {
        QDialog::closeEvent( event );
    }

    return;
}


void Dialog::keyPressEvent( QKeyEvent *event  )
{
    if ( (Qt::ControlModifier == event->modifiers()) \
         && (Qt::Key_S == event->key()) \
       )
    {
        saveMemoData();

        return;
    }

    switch ( event->key() )
    {
        case Qt::Key_Escape:
        {
            hideToSystemTrayIcon();

            break;
        }

        default:
        {
            QDialog::keyPressEvent( event );

            break;
        }
    }

    return;
}


void Dialog::initVars()
{
    m_queryFlag = false;
    m_exitFlag = false;
    m_titleDate.clear();
    m_gifUrls.clear();
    m_gifFileList.clear();

    m_isSysTrayAvailable = QSystemTrayIcon::isSystemTrayAvailable();

    m_settings = new (std::nothrow) Settings();

    if ( NULL != m_settings )
    {
        connect( m_settings, SIGNAL(updateSetting()), this, SLOT(updateSetting()) );
    }

    m_saveMemo = new (std::nothrow) SaveMemo();

    if ( NULL != m_saveMemo )
    {
        refreshContent();
    }

//    ui->textEdit->setContextMenuPolicy( Qt::NoContextMenu );
    m_helpInfo = new (std::nothrow) HelpInfo();

    if ( NULL != m_helpInfo )
    {
        connect( m_helpInfo, SIGNAL(hideMainWindow()), this, SLOT(hideToSystemTrayIcon()) );
    }

    m_queryMemo = new (std::nothrow) QueryMemo();

    if ( NULL != m_queryMemo )
    {
        connect( m_queryMemo, SIGNAL(selectedDate(const QString)), this, SLOT(selectedDate(const QString)) );
    }

    connect( ui->textEdit, SIGNAL(addAnimation(const QString)), this, SLOT(addAnimation(const QString)) );

    return;
}


void Dialog::initSystemTray()
{
    if ( !m_isSysTrayAvailable )
    {
        return;
    }

    m_trayIcon = new (std::nothrow) QSystemTrayIcon( this );

    if ( NULL != m_trayIcon )
    {
        QIcon icon = QIcon( ":/icon.png" );
        m_trayIcon->setIcon( icon );
        m_trayIcon->setToolTip( "随手记.." );

        m_trayIconMenu = new (std::nothrow) QMenu( this );

        if ( NULL != m_trayIconMenu )
        {
            m_showMainWindow = new (std::nothrow) QAction( "显示主窗口", this );

            if ( NULL != m_showMainWindow )
            {
                connect( m_showMainWindow, SIGNAL(triggered()), this, SLOT(topShow()) );
                m_showMainWindow->setIcon( QIcon(":/window.png") );
                m_trayIconMenu->addAction( m_showMainWindow );
                m_trayIconMenu->addSeparator();
            }

            m_configAction = new (std::nothrow) QAction( "保存设置", this );

            if ( NULL != m_configAction )
            {
                connect( m_configAction, SIGNAL(triggered()), this, SLOT(settingSlot()) );
                m_configAction->setIcon( QIcon(":/setting.png") );
                m_trayIconMenu->addAction( m_configAction );
                m_trayIconMenu->addSeparator();
            }

            m_exitAction = new (std::nothrow) QAction( "退出", this );

            if ( NULL != m_exitAction )
            {
                connect( m_exitAction, SIGNAL(triggered()), this, SLOT(exitSlot()) );
                m_exitAction->setIcon( QIcon(":/exit.png") );
                m_trayIconMenu->addAction( m_exitAction );
            }

            m_trayIcon->setContextMenu( m_trayIconMenu );
        }

        connect( m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activated(QSystemTrayIcon::ActivationReason)) );
    }

    return;
}


void Dialog::initShortcut()
{
    QxtGlobalShortcut *showShortcut = new QxtGlobalShortcut( this );
    showShortcut->setShortcut( QKeySequence("Ctrl+F12") );
    connect( showShortcut, SIGNAL(activated()), this, SLOT(topShow()) );

    QxtGlobalShortcut *settingShortcut = new QxtGlobalShortcut( this );
    settingShortcut->setShortcut( QKeySequence("Ctrl+F11") );
    connect( settingShortcut, SIGNAL(activated()), this, SLOT(settingSlot()) );

    QxtGlobalShortcut *exitShortcut = new QxtGlobalShortcut( this );
    exitShortcut->setShortcut( QKeySequence("Ctrl+F4") );
    connect( exitShortcut, SIGNAL(activated()), this, SLOT(exitSlot()) );

    QxtGlobalShortcut *captrueScreenShortcut = new QxtGlobalShortcut( this );
    captrueScreenShortcut->setShortcut( QKeySequence("Ctrl+Alt+S") );
    connect( captrueScreenShortcut, SIGNAL(activated()), this, SLOT(on_snapshotPushButton_clicked()) );

    return;
}


void Dialog::initConfigure()
{
    QRegExp rx( "[0-9]+$" );
    QString value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value" );

    connect( &m_saveTimer, SIGNAL(timeout()), this, SLOT(saveMemoData()) );

    if ( rx.exactMatch(value) )
    {
        if ( 0 < value.toInt() )
        {
            if ( 100 > value.toInt() )
            {
                m_saveTimer.start( value.toInt() * 60 * 1000 );
            }
            else
            {
                m_saveTimer.start( 99 * 60 * 1000 );
                ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value", "99" );
            }
        }
        else
        {
            m_saveTimer.stop();
        }
    }
    else
    {
        m_saveTimer.start( DEFAULT_SAVE_INTERVAL );
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value", "5" );
    }

    value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SavePath/Value" );

    if ( value.isEmpty() )
    {
#ifdef Q_OS_WIN32
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SavePath/Value", "c:/memos" );
#else
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SavePath/Value", "/home/hans/memos" );
#endif
    }

    return;
}


void Dialog::activeGifMovie( const QString &data )
{
    if ( data.isEmpty() )
    {
        return;
    }

    int begin_pos = 0, end_pos = 0;
    QString gifFile;
    QStringList gifList;

    gifList.clear();

    while ( 0 <= begin_pos )
    {
        begin_pos = data.indexOf( "<IMG src=\"", end_pos, Qt::CaseInsensitive );

        if ( 0 > begin_pos )
        {
            break;
        }

        begin_pos += 10;
        end_pos = data.indexOf( "\"", begin_pos );
        gifFile = data.mid( begin_pos, end_pos - begin_pos );

        if ( !gifFile.isEmpty() )
        {
            addAnimation( gifFile );
            gifList << gifFile;
        }
    }

    return;
}


void Dialog::addAnimation( const QString gifFile )
{
    if ( m_gifFileList.contains(gifFile) )
    {
        return;
    }

    QFile file( gifFile );

    if ( !file.exists() )
    {
        return;
    }

    if ( file.open(QFile::ReadOnly) )
    {
        char sign[8] = { 0 };

        file.read( sign, 8 );
        file.close();

        if ( ('G' != sign[0]) || ('I' != sign[1]) || ('F' != sign[2]) )
        {
            return;
        }
    }

    m_gifFileList.append( gifFile );

    QMovie* movie = new (std::nothrow) QMovie( this );

    if ( NULL != movie )
    {
        movie->setFileName( gifFile );
        movie->setCacheMode( QMovie::CacheNone );

        m_gifUrls.insert( movie, QUrl(gifFile) );

        connect( movie, SIGNAL(frameChanged(int)), this, SLOT(subAnimate(int)) );
        movie->start();
    }

    return;
}


void Dialog::subAnimate( int frameNumber )
{
    Q_UNUSED( frameNumber )

    if ( m_gifUrls.isEmpty() )
    {
        return;
    }

    QMovie* movie = qobject_cast <QMovie *> ( sender() );

    if ( NULL != movie )
    {
        ui->textEdit->document()->addResource( QTextDocument::ImageResource, m_gifUrls.value(movie), movie->currentPixmap() );
        ui->textEdit->setLineWrapColumnOrWidth( ui->textEdit->lineWrapColumnOrWidth() );
    }

    return;
}


void Dialog::hideToSystemTrayIcon()
{
    if ( !isHidden() )
    {
        hide();
    }

    if ( NULL != m_trayIcon )
    {
        if ( !m_trayIcon->isVisible() )
        {
            m_trayIcon->show();
        }
    }

    saveMemoData();

    if ( m_queryFlag )
    {
        refreshTitle();
        refreshContent();
        m_queryFlag = false;
    }

    return;
}


void Dialog::topShow()
{
    if ( m_titleDate != QDate::currentDate().toString( "yyyy-MM-dd" ) )
    {
        m_titleDate = QDate::currentDate().toString( "yyyy-MM-dd" );
        refreshTitle( m_titleDate );
        refreshContent( m_titleDate );;
    }

    showNormal();
    activateWindow();

    return;
}


void Dialog::updateSetting()
{
    QString value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value" );

    if ( !value.isEmpty() )
    {
        if ( 0 == value.toInt() )
        {
            m_saveTimer.stop();
        }
        else if ( (value.toInt() * 60 * 1000) != m_saveTimer.interval() )
        {
            m_saveTimer.setInterval( value.toInt() * 60 * 1000 );
        }
    }

    value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SavePath/Value" );

    if ( !value.isEmpty() )
    {
        if ( NULL != m_saveMemo )
        {
            m_saveMemo->updateSavedPath( value);
        }
    }

    return;
}


void Dialog::settingSlot()
{
    if ( NULL != m_settings )
    {
        QRegExp rx( "[0-9]+$");
        QString value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value" );

        if ( rx.exactMatch(value) )
        {
            int i = value.toInt();

            if ( (0 < i) && (100 > i) )
            {
                m_settings->setLineEditValue( value );

                if ( (1000 * 60 * i) != m_saveTimer.interval() )
                {
                    m_saveTimer.setInterval( DEFAULT_SAVE_INTERVAL );
                }
            }
            else
            {
                m_saveTimer.stop();
                ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value", "0" );
                m_settings->setLineEditValue( "0" );
            }
        }
        else
        {
            m_settings->setLineEditValue( "5" );
            ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value", "5" );

            if ( DEFAULT_SAVE_INTERVAL != m_saveTimer.interval() )
            {
                m_saveTimer.setInterval( DEFAULT_SAVE_INTERVAL );
            }
        }

        value = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SavePath/Value" );
        m_settings->setPathLineEditValue( value );

        m_settings->show();
    }

    return;
}


void Dialog::exitSlot()
{
    saveMemoData();

    m_exitFlag = true;
    close();

    return;
}


void Dialog::refreshTitle( const QString &date )
{
    if ( !date.isEmpty() )
    {
        m_titleDate = date;
    }
    else
    {
        m_titleDate = QDate::currentDate().toString( "yyyy-MM-dd" );
    }

    QString title = QString( "随手记 -- %1" ).arg( m_titleDate );
    setWindowTitle( title );

    return;
}


void Dialog::refreshContent( const QString& contentDate )
{
    if ( NULL == m_saveMemo )
    {
        return;
    }

    QString value = m_saveMemo->getData( contentDate );

    ui->textEdit->setHtml( value );
    activeGifMovie( value );

    return;
}


void Dialog::activated( QSystemTrayIcon::ActivationReason reason )
{
    if ( QSystemTrayIcon::Trigger == reason )
    {
        topShow();
    }

    return;
}


void Dialog::saveMemoData()
{
    if ( NULL == m_saveMemo )
    {
        return;
    }

    m_saveMemo->saveData( ui->textEdit->toHtml(), m_titleDate );

    return;
}


void Dialog::on_helpPushButton_clicked()
{
    if ( NULL != m_helpInfo )
    {
        m_helpInfo->show();
    }

    return;
}


void Dialog::on_insertImgPushButton_clicked()
{
    QString imgName = QFileDialog::getOpenFileName( this, "请选择一个图片文件", DEFAULT_DIRECTORY, "*.png *.jpg *.jpeg *.bmp *.gif *.ico" );

    if ( imgName.isEmpty() )
    {
        return;
    }

    QFile file;

    file.setFileName( imgName );

    if ( file.open(QFile::ReadOnly) )
    {
        QByteArray data = file.readAll();

        file.close();

        if ( !data.isEmpty() && (NULL != m_saveMemo) )
        {
            int lastSlash = imgName.lastIndexOf( '/' );

            if ( 0 > lastSlash )
            {
                lastSlash = imgName.lastIndexOf( '\\' );
            }

            QString rawName = imgName;

            if ( 0 < lastSlash )
            {
                rawName = imgName.mid( lastSlash );
            }

            QString picture = m_saveMemo->savePicture( data, rawName );
            QString img = QString( "<IMG src=\"%1\"/>" ).arg( picture );

            ui->textEdit->textCursor().insertHtml( img );

            if ( ('G' == data[0]) && ('I' == data[1]) && ('F' == data[2]) )
            {
                addAnimation( picture );
            }
        }
    }

    return;
}


void Dialog::onCompleteCature( QPixmap captureImage )
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setPixmap( captureImage );

    return;
}


void Dialog::on_snapshotPushButton_clicked()
{
    CaptureScreen *captureScreen = new (std::nothrow) CaptureScreen();

    if ( NULL != captureScreen )
    {
        connect( captureScreen, SIGNAL(completeCature(QPixmap)), this, SLOT(onCompleteCature(QPixmap)) );
        captureScreen->show();
    }

    return;
}


void Dialog::selectedDate( const QString date )
{
    if ( NULL != m_saveMemo )
    {
        m_queryFlag = true;
        refreshTitle( date );
        refreshContent( date );
    }

    return;
}


void Dialog::on_calendarPushButton_clicked()
{
    if ( NULL != m_queryMemo )
    {
        saveMemoData();
        m_queryMemo->setDateToToday();
        m_queryMemo->show();
    }

    return;
}


void Dialog::on_textColorPushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( !selectedText.isEmpty() )
    {
        QColor color = QColorDialog::getColor();

        if ( color.isValid() )
        {
            ui->textEdit->setTextColor( color );
        }
    }

    ui->textEdit->setFocus();

    return;
}


void Dialog::on_textBackgroundColorPushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( !selectedText.isEmpty() )
    {
        QColor color = QColorDialog::getColor();

        if ( color.isValid() )
        {
            ui->textEdit->setTextBackgroundColor( color );
        }
    }

    ui->textEdit->setFocus();

    return;
}


void Dialog::on_boldTextPushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( selectedText.isEmpty() )
    {
        return;
    }

    if ( QFont::Normal == ui->textEdit->fontWeight() )
    {
        ui->textEdit->setFontWeight( QFont::Bold );
    }
    else if ( QFont::Bold == ui->textEdit->fontWeight() )
    {
        ui->textEdit->setFontWeight( QFont::Normal );
    }

    ui->textEdit->setFocus();

    return;
}


void Dialog::on_italicTextPushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( selectedText.isEmpty() )
    {
        return;
    }

    if ( ui->textEdit->fontItalic() )
    {
        ui->textEdit->setFontItalic( false );
    }
    else
    {
        ui->textEdit->setFontItalic( true );
    }

    ui->textEdit->setFocus();

    return;
}


void Dialog::on_underlineTextPushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( selectedText.isEmpty() )
    {
        return;
    }

    if ( ui->textEdit->fontUnderline() )
    {
        ui->textEdit->setFontUnderline( false );
    }
    else
    {
        ui->textEdit->setFontUnderline( true );
    }

    ui->textEdit->setFocus();

    return;
}


void Dialog::on_increaseSizePushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( selectedText.isEmpty() )
    {
        return;
    }

    qreal size = ui->textEdit->currentFont().pointSize() + 2;

    if ( 50 < size )
    {
        return;
    }

    ui->textEdit->setFontPointSize( size );
    ui->textEdit->setFocus();

    return;
}


void Dialog::on_decreaseSizePushButton_clicked()
{
    QString selectedText = ui->textEdit->textCursor().selectedText();

    if ( selectedText.isEmpty() )
    {
        return;
    }

    qreal size = ui->textEdit->currentFont().pointSize() - 2;

    if ( 5 > size )
    {
        return;
    }

    ui->textEdit->setFontPointSize( size );
    ui->textEdit->setFocus();

    return;
}
