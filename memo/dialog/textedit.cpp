/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "textedit.h"
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QImage>
#include <QMenu>
#include <QTextDocumentFragment>
#include "settings.h"

#define DROP_CONTENT_HTML_START "<!--StartFragment -->"
#define DROP_CONTENT_HTML_END   "<!--EndFragment-->"

TextEdit::TextEdit( QWidget *parent )
{
    if ( NULL != parent )
    {
        setParent( parent );
    }

    m_savePath.clear();

    installEventFilter( this );
}


void TextEdit::dropEvent( QDropEvent *event )
{
    if ( NULL == event )
    {
        return;
    }

    if ( NULL != event->mimeData() )
    {
        if ( event->mimeData()->hasHtml() )
        {
            insertHtmlFragment( event->mimeData()->html() );
            event->ignore();

            return;
        }
        else if ( event->mimeData()->hasText() )   // 显示拖入的图片
        {
            insertTextFragment( event->mimeData()->text() );
            event->ignore();

            return;
        }
    }

    QTextEdit::dropEvent( event );

    return;
}


bool TextEdit::eventFilter( QObject *obj, QEvent *event )
{
    Q_UNUSED( obj )

    if ( QEvent::KeyPress == event->type() )
    {
        QKeyEvent *keyEvent = static_cast <QKeyEvent *> (event);

        if ( keyEvent->matches(QKeySequence::Paste) )
        {
            QClipboard *clipboard = QApplication::clipboard();

            if ( clipboard->mimeData()->hasImage() )
            {
                loadImageSavePath();

                QImage imgData = clipboard->image();
                QString imgFile = QString( "%1/img/%2.png" ).arg( m_savePath ).arg( QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss_zzz") );

                imgData.save( imgFile );

                QString imgHtml = QString( "<IMG src=\"%1\"/>" ).arg( imgFile );

                textCursor().insertHtml( imgHtml );

                return true;
            }
        }
    }

    return QTextEdit::eventFilter( obj, event );
}


void TextEdit::contextMenuEvent( QContextMenuEvent *event )
{
    static __thread QMenu *menu = NULL;

    if ( NULL == menu )
    {
        menu = createStandardContextMenu( this );
    }

    if ( NULL != menu )
    {
//        menu->setAttribute( Qt::WA_DeleteOnClose );
        menu->popup( event->globalPos() );
    }

    event->ignore();

    return;
}


bool TextEdit::isImageFile( const QString fileName )
{
    QImage img;

    img.load( fileName );

    if ( img.isNull() )
    {
        return false;
    }

    return true;
}


void TextEdit::loadImageSavePath()
{
    m_savePath = Settings::getSavedPath();

    if ( m_savePath.isEmpty() )
    {
#ifdef Q_OS_WIN32
        m_savePath = QString( "c:/memos" );
#else
        m_savePath = QString( "/home/hans/memos" );
#endif
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SavePath/Value", m_savePath );
    }
    else
    {
        m_savePath.replace( '\\', '/' );

        if ( '/' == m_savePath.right(1) )
        {
            m_savePath = m_savePath.left( m_savePath.length() - 1 );
        }
    }

    QDir dir;

    if ( !dir.exists(m_savePath + "/img") )
    {
        dir.mkpath( m_savePath + "/img" );
    }

    return;
}


QString TextEdit::copyImgFileToLocal( const QString oldFile )
{
    QString newFile;

    newFile.clear();

    if ( oldFile.isEmpty() )
    {
        return newFile;
    }

    QFile file( oldFile );

    if ( !file.exists() )
    {
        return newFile;
    }

    int pos = oldFile.lastIndexOf( '/' );

    if ( 0 > pos )
    {
        pos = oldFile.lastIndexOf( '\\' );
    }

    QString fileSuffix;

    fileSuffix.clear();

    if ( 0 < pos )
    {
        pos++;
        fileSuffix = oldFile.mid( pos );
        pos = fileSuffix.lastIndexOf( '.' );

        if ( 0 < pos )
        {
            fileSuffix = fileSuffix.mid( pos );
        }
        else
        {
            fileSuffix.clear();
        }
    }

    loadImageSavePath();

    newFile = QString( "%1/img/%2_%3" ).arg( m_savePath ).arg( QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss_zzz") ).arg( fileSuffix );

    if ( !QFile::copy(oldFile, newFile) )
    {
        newFile.clear();

        return newFile;
    }

    emit addAnimation( newFile );

    return newFile;
}


// 将临时图片拷贝到目标目录 img
QString TextEdit::resloveImageTag( const QString data, const bool isHtml )
{
    m_newImageTag = data;

    if ( m_newImageTag.isEmpty() )
    {
        return m_newImageTag;
    }

    int begin_pos = 0, end_pos = 0;

    QString oldPicFile, purePicFile, newPicFile;

    if ( Q_LIKELY(isHtml) )
    {
        while ( 0 <= begin_pos )
        {
            begin_pos = data.indexOf( "<IMG src=\"", end_pos, Qt::CaseInsensitive );

            if ( 0 > begin_pos )
            {
                break;
            }

            begin_pos += 10;  // length of  <IMG src="
            end_pos = data.indexOf( "\"", begin_pos );
            oldPicFile = data.mid( begin_pos, end_pos - begin_pos );

            if ( 4 < oldPicFile.length() )
            {
                purePicFile = oldPicFile;
                purePicFile.remove( "file:///" );
                newPicFile = copyImgFileToLocal( purePicFile );
                m_newImageTag.replace( oldPicFile, newPicFile );
            }
        }
    }
    else
    {
        QStringList fileList = m_newImageTag.split( '\n' );

        for ( int i = 0; i < fileList.count(); i++ )
        {
            oldPicFile = fileList[i];

            if ( 4 < oldPicFile.length() )
            {
                purePicFile = oldPicFile;
                purePicFile.remove( "file:///" );

                if ( isImageFile(purePicFile) )
                {
                    newPicFile = copyImgFileToLocal( purePicFile );

                    QString imgFile = QString( "<IMG src=\"%1\"/>" ).arg( newPicFile );

                    textCursor().insertHtml( imgFile );
                    m_newImageTag.remove( oldPicFile );
                }
            }
        }
    }

    return m_newImageTag;
}


void TextEdit::insertHtmlFragment( const QString data )
{
    if ( data.isEmpty() )
    {
        return;
    }

    int startPos = data.indexOf( DROP_CONTENT_HTML_START );

    if ( 0 > startPos )
    {
        return;
    }

    startPos += strlen( DROP_CONTENT_HTML_START );

    int endPos = data.indexOf( DROP_CONTENT_HTML_END );

    if ( 0 > endPos )
    {
        return;
    }

    m_htmlFragment = data.mid( startPos, endPos - startPos );
    m_htmlFragment = resloveImageTag( m_htmlFragment );
    textCursor().insertHtml( m_htmlFragment );

    return;
}


void TextEdit::insertTextFragment( const QString data )
{
    if ( data.isEmpty() )
    {
        return;
    }

    m_textFragment = resloveImageTag( data, false );
    textCursor().insertText( m_textFragment );

    return;
}


QMenu *TextEdit::createStandardContextMenu( QWidget *parent )
{
    QMenu *popupMenu = new (std::nothrow) QMenu( parent );

    if ( NULL == popupMenu )
    {
        return NULL;
    }

    popupMenu->setObjectName( QLatin1String("qt_edit_menu") );

    QAction *action = NULL;

    if ( !isReadOnly() )
    {
        action = popupMenu->addAction( tr("&撤消\t") + QKeySequence(QKeySequence::Undo).toString() );
        action->setEnabled( isUndoRedoEnabled() );
        action->setIcon( QIcon(":/undo.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(undo()) );

        action = popupMenu->addAction( tr("&重做\t") + QKeySequence(QKeySequence::Redo).toString() );
        action->setEnabled( isUndoRedoEnabled());
        action->setIcon( QIcon(":/redo.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(redo()) );

        popupMenu->addSeparator();

        action = popupMenu->addAction( tr("&剪切\t") + QKeySequence(QKeySequence::Cut).toString() );
        action->setIcon( QIcon(":/cut.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(cut()) );

        action = popupMenu->addAction( tr("&复制\t") + QKeySequence(QKeySequence::Copy).toString() );
        action->setIcon( QIcon(":/copy.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(copy()) );

        action = popupMenu->addAction( tr("&粘贴\t") + QKeySequence(QKeySequence::Paste).toString() );
        action->setIcon( QIcon(":/paste.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(pasteClipboardContent()) );

        action = popupMenu->addAction( tr("&删除\t") + QKeySequence(QKeySequence::Delete).toString() );
        action->setIcon( QIcon(":/delete.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(removeSelectedText()) );

        popupMenu->addSeparator();

        action = popupMenu->addAction( tr("&全选\t") + QKeySequence(QKeySequence::SelectAll).toString() );
        action->setIcon( QIcon(":/select_all.png") );
        connect( action, SIGNAL(triggered()), this, SLOT(selectAll()) );
    }

    return popupMenu;
}


void TextEdit::pasteClipboardContent()
{
    QKeyEvent pasteKey( QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier );
    QCoreApplication::sendEvent( this, &pasteKey );

    return;
}


void TextEdit::removeSelectedText()
{
    textCursor().removeSelectedText();

    return;
}
