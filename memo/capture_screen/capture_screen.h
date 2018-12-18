/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#ifndef CAPTURE_SCREEN_H
#define CAPTURE_SCREEN_H

#include <QWidget>
#include <QPainter>
#include <QRect>

class CaptureScreen : public QWidget
{
    Q_OBJECT

public:
    CaptureScreen( QWidget *parent = 0 );
    ~CaptureScreen();

    QRect getRect( const QPoint &beginPoint, const QPoint &endPoint );

private:
    void initWindow();
    void loadBackgroundPixmap();

    void mousePressEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent *event );
    void mouseDoubleClickEvent( QMouseEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void paintEvent( QPaintEvent *event );

private:
    bool m_isMousePress;
    int m_screenwidth;
    int m_screenheight;
    QRect m_selectedRect;

    QPainter m_painter;
    QPoint m_beginPoint, m_endPoint, m_tmpPoint;
    QPixmap m_loadPixmap, m_capturePixmap;

signals:
    void completeCature( QPixmap catureImage );
};

#endif // CAPTURE_SCREEN_H
