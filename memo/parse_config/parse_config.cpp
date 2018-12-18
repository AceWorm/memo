/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include <parse_config.h>
#include <QSettings>

QString ParseConfigFile::getFieldValue( const QString &fileName, const QString &fieldName )
{
    QSettings settting( fileName, QSettings::IniFormat );
    QString value = settting.value( fieldName, "" ).toString();

    return value;
}


void ParseConfigFile::setFieldValue( const QString &fileName, const QString &fieldName, const QString& fieldValue )
{
    QSettings settting( fileName, QSettings::IniFormat );
    settting.setValue( fieldName, fieldValue );

    return;
}
