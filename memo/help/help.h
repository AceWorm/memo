/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef HELP_H
#define HELP_H

#include <QDialog>

namespace Ui {
class HelpInfo;
}

class HelpInfo : public QDialog
{
    Q_OBJECT

public:
    explicit HelpInfo( QWidget *parent = 0 );
    ~HelpInfo();

private slots:
    void on_mailLabel_linkActivated(const QString &link);

private:
    Ui::HelpInfo *ui;

signals:
    void hideMainWindow();
};

#endif // HELP_H
