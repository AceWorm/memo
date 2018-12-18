/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>

QString Settings::m_savedPath = QString();

Settings::Settings(QWidget *parent) : QDialog( parent ), ui( new Ui::Settings )
{
    ui->setupUi( this );
    setFixedSize( size() );

    setWindowFlags( Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint );
    setAttribute( Qt::WA_QuitOnClose, false );

    ui->lineEdit->setValidator( new QIntValidator(0, 100, this) );
    ui->pushButton->setFocus();
}


Settings::~Settings()
{
    delete ui;
}


void Settings::setLineEditValue( const QString &value )
{
    ui->lineEdit->setText( value );

    return;
}


void Settings::setPathLineEditValue( const QString &value )
{
    ui->pathLineEdit->setText( value );

    return;
}


QString Settings::getSavedPath()
{
    m_savedPath = ParseConfigFile::getFieldValue( CONFIG_FILE_NAME, "SavePath/Value" );

    return m_savedPath;
}


void Settings::on_pushButton_clicked()
{
    if ( !ui->lineEdit->text().isEmpty() )
    {
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SaveInterval/Value", ui->lineEdit->text() );
    }

    if ( !ui->pathLineEdit->text().isEmpty() )
    {
        ParseConfigFile::setFieldValue( CONFIG_FILE_NAME, "SavePath/Value", ui->pathLineEdit->text() );
    }

    hide();
    emit updateSetting();

    return;
}


void Settings::on_browsePushButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory( this );

    ui->pathLineEdit->setText( path );

    return;
}
