/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef QUERY_MEMO_H
#define QUERY_MEMO_H

#include <QDialog>

namespace Ui {
class QueryMemo;
}

class QueryMemo : public QDialog
{
    Q_OBJECT

public:
    explicit QueryMemo( QWidget *parent = 0 );
    ~QueryMemo();

    void setDateToToday();

private:
    Ui::QueryMemo *ui;
    QString m_selectedDate;

private slots:
    void on_pushButton_clicked();
    void on_todayPushButton_clicked();

signals:
    void selectedDate( const QString date );
};

#endif // QUERY_MEMO_H
