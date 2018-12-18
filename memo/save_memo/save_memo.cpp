/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "save_memo.h"
#include <QDate>
#include <QDir>
#include <QMessageBox>
#include "parse_config.h"

#ifdef Q_OS_WIN32
#define CONFIG_FILE_NAME "c:/memo_config.ini"
#else
#define CONFIG_FILE_NAME "/home/hans/memo_config.ini"
#endif

SaveMemo::SaveMemo()
{
    m_updateFlag = false;
    m_lastDate.clear();
    m_lastData.clear();

    m_savePath = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SavePath/Value" );

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
}


QString SaveMemo::savePicture( const QByteArray &data, const QString &fileName )
{
    QString img_path = QString( "%1/img" ).arg( m_savePath );

    QDir dir;

    if ( !dir.exists(img_path) )
    {
        dir.mkpath( img_path );
    }

    QString newName = fileName;

    if ( !newName.isEmpty() )
    {
        newName = newName.replace( "/", "_" );
        newName = newName.replace( "\\", "_" );
    }

    QString img_name = QString( "%1/%2_%3" ).arg( img_path ).arg( QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss_zzz") ).arg( newName );

    QFile file;

    file.setFileName( img_name );

    if ( file.open(QFile::WriteOnly) )
    {
        file.write( data );
        file.close();
    }

    return img_name;
}


void SaveMemo::saveData( const QString &data , const QString& date )
{
    if ( (m_lastDate == date) && (m_lastData == data) )
    {
        return;
    }

    if ( !date.isEmpty() )
    {
        m_lastDate = date;
    }
    else
    {
        m_lastDate = QDate::currentDate().toString( "yyyy-MM-dd" );
    }

    m_lastData = data;

    QDir dir;

    if ( !dir.exists(m_savePath) )
    {
        dir.mkpath( m_savePath );
    }

    QString file_name = QString( "%1/%2.html" ).arg( m_savePath ).arg( m_lastDate );

    if ( file_name != m_file.fileName() )
    {
        m_file.setFileName( file_name );
    }

#if 0
    if ( m_updateFlag && m_file.exists() )
    {
        if ( QMessageBox::Yes == QMessageBox::warning(NULL, "警告", "目标文件已经存在于新指定目录中，是否覆盖？", QMessageBox::Yes | QMessageBox::No) )
        {
            if ( m_file.open(QFile::WriteOnly) )
            {
                m_lastData.remove( '\r' );
                m_lastData.replace( '\n', "\r\n" );
                m_file.write( m_lastData.toUtf8() );
                m_file.flush();
                m_file.close();
            }
        }
        else
        {
            if ( m_file.open(QFile::WriteOnly | QFile::Append) )
            {
                m_lastData.remove( '\r' );
                m_lastData.replace( '\n', "\r\n" );
                m_file.write( m_lastData.toUtf8() );
                m_file.flush();
                m_file.close();
            }
        }

        m_updateFlag = false;
    }
    else
#endif
    {
        if ( m_file.open(QFile::WriteOnly) )
        {
            m_lastData.remove( '\r' );
            m_lastData.replace( '\n', "\r\n" );
            m_file.write( m_lastData.toUtf8() );
            m_file.flush();
            m_file.close();
        }
    }

    return;
}


QString SaveMemo::getData( const QString date )
{
    QString file_name;

    file_name = m_savePath;
    m_lastData.clear();

    if ( date.isEmpty() )
    {
        file_name += QDate::currentDate().toString( "/yyyy-MM-dd.html" );
    }
    else
    {
        file_name += QString( "/%1.html" ).arg( date );
    }

    if ( file_name != m_file.fileName() )
    {
        m_file.setFileName( file_name );
    }

    if ( m_file.open(QFile::ReadOnly) )
    {
        m_file.seek( 0 );
        m_lastData = m_file.readAll().constData();
        m_file.close();
    }

    return m_lastData;
}


void SaveMemo::updateSavedPath( const QString path )
{
    if ( !path.isEmpty() )
    {
        m_updateFlag = true;
        m_savePath = path;
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SavePath/Value", m_savePath );
    }

    return;
}
