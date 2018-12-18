/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "help.h"
#include "ui_help.h"
#include <QFileDialog>
#include <QDesktopServices>

HelpInfo::HelpInfo(QWidget *parent) : QDialog( parent ), ui( new Ui::HelpInfo )
{
    ui->setupUi( this );
    setFixedSize( size() );

    setWindowFlags( Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint );
    setAttribute( Qt::WA_QuitOnClose, false );
}


HelpInfo::~HelpInfo()
{
    delete ui;
}


void HelpInfo::on_mailLabel_linkActivated( const QString &link )
{
    bool flag = QDesktopServices::openUrl( QUrl(link) );

    if ( flag )
    {
        hide();
        emit hideMainWindow();
    }

    return;
}
