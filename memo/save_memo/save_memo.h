/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef SAVE_MEMO_H
#define SAVE_MEMO_H

#include <QString>
#include <QFile>
#include <QImage>

class SaveMemo
{
public:
    SaveMemo();

    QString savePicture( const QByteArray &data, const QString &fileName = QString() );
    void saveData( const QString &data, const QString &date = QString() );
    QString getData( const QString date = QString() );
    void updateSavedPath( const QString path );

private:
    bool m_updateFlag;
    QFile m_file;
    QString m_savePath;
    QString m_lastDate, m_lastData;
};

#endif // SAVE_MEMO_H
