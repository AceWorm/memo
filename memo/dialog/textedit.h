/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QDropEvent>
#include <QContextMenuEvent>
#include <QKeysequence>

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit( QWidget *parent = 0 );

protected:
    void dropEvent( QDropEvent *event );
    bool eventFilter( QObject *obj, QEvent *event );
    void contextMenuEvent( QContextMenuEvent *event );

private:
    QString m_savePath;
    QString m_newImageTag;
    QString m_textFragment, m_htmlFragment;

private:
    bool isImageFile( const QString fileName );
    void loadImageSavePath();
    QString copyImgFileToLocal( const QString oldFile );
    QString resloveImageTag( const QString data, const bool isHtml = true );
    void insertHtmlFragment( const QString data );
    void insertTextFragment( const QString data );

    QMenu *createStandardContextMenu( QWidget *parent );

private slots:
    void pasteClipboardContent();
    void removeSelectedText();

signals:
    void addAnimation( const QString gifFile );
};

#endif // TEXTEDIT_H
