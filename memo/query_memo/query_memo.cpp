/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "query_memo.h"
#include "ui_query_memo.h"
#include <QCalendarWidget>
#include <QTextCharFormat>

QueryMemo::QueryMemo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryMemo)
{
    ui->setupUi( this );
    setFixedSize( size() );

    setWindowFlags( Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint );
    setAttribute( Qt::WA_QuitOnClose, false );

    m_selectedDate.clear();
}


QueryMemo::~QueryMemo()
{
    delete ui;
}


void QueryMemo::setDateToToday()
{
    on_todayPushButton_clicked();

    return;
}


void QueryMemo::on_pushButton_clicked()
{
    m_selectedDate = ui->calendarWidget->selectedDate().toString( "yyyy-MM-dd" );

    emit selectedDate( m_selectedDate );
    close();

    return;
}


void QueryMemo::on_todayPushButton_clicked()
{
    QDate date = QDate::currentDate();

    ui->calendarWidget->setSelectedDate( date );

    QTextCharFormat format;

    format.setFontPointSize( 15 );
    format.setFontItalic( true );
    format.setForeground( QColor(255, 255, 255) );
    format.setBackground( Qt::magenta );

    ui->calendarWidget->setDateTextFormat( date, format );
    ui->calendarWidget->setFocus();

    return;
}
