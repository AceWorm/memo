/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "parse_config.h"

#ifdef Q_OS_WIN32
#define CONFIG_FILE_NAME "c:/memo_config.ini"
#else
#define CONFIG_FILE_NAME "/home/hans/memo_config.ini"
#endif

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings( QWidget *parent = 0 );
    ~Settings();

    void setLineEditValue( const QString &value );
    void setPathLineEditValue( const QString &value );
    static QString getSavedPath();

private slots:
    void on_pushButton_clicked();
    void on_browsePushButton_clicked();

private:
    Ui::Settings *ui;
    static QString m_savedPath;

signals:
    void updateSetting();
};

#endif // SETTINGS_H
