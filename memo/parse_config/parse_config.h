/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H

#include <QString>

class ParseConfigFile
{
public:
    static QString getFieldValue( const QString& fileName, const QString& fieldName );
    static void setFieldValue( const QString& fileName, const QString& fieldName, const QString& fieldValue );
};

#endif  // PARSE_CONFIG_H
