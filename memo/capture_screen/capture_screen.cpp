/**
 * @brief  Created by civen163@163.com
 * @date   yyyy-MM-dd
 * @author Civen
 */
#include "capture_screen.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QScreen>

CaptureScreen::CaptureScreen( QWidget *parent ) : QWidget( parent ), m_isMousePress( false )
{
    m_selectedRect.setRect( 0, 0, 0, 0 );

    initWindow();
    loadBackgroundPixmap();
}


CaptureScreen::~CaptureScreen()
{
}


void CaptureScreen::initWindow()
{
    this->setMouseTracking( true );
    this->setWindowFlags( Qt::FramelessWindowHint );
    setAttribute( Qt::WA_QuitOnClose, false );
    setWindowState( Qt::WindowActive | Qt::WindowFullScreen );

    return;
}


void CaptureScreen::loadBackgroundPixmap()
{
    QScreen *screen = QGuiApplication::primaryScreen();

    m_loadPixmap = screen->grabWindow( QApplication::desktop()->winId() );
    m_screenwidth = m_loadPixmap.width();
    m_screenheight = m_loadPixmap.height();

    return;
}


void CaptureScreen::mousePressEvent( QMouseEvent *event )
{
    if ( Qt::LeftButton == event->button() )
    {
        m_isMousePress = true;
        m_beginPoint = event->pos();
    }
    else if ( Qt::RightButton == event->button() )
    {
        event->ignore();
        emit completeCature( m_capturePixmap );
        close();
    }

    QWidget::mousePressEvent( event );

    return;
}


void CaptureScreen::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_isMousePress )
    {
        m_endPoint = event->pos();

        m_tmpPoint = m_endPoint - m_beginPoint;

        if ( 8 < m_tmpPoint.manhattanLength() )
        {
            update();
        }
    }

    QWidget::mouseMoveEvent(event);

    return;
}


void CaptureScreen::mouseReleaseEvent( QMouseEvent *event )
{
    m_endPoint = event->pos();
    m_isMousePress = false;

    QWidget::mouseReleaseEvent( event );

    return;
}


void CaptureScreen::mouseDoubleClickEvent( QMouseEvent* event )
{
    if ( m_selectedRect.contains(event->pos(), true) )
    {
        event->ignore();
        emit completeCature( m_capturePixmap );
        close();
    }

    QWidget::mouseDoubleClickEvent( event );

    return;
}


void CaptureScreen::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event )

    m_painter.begin( this );

    QColor shadowColor = QColor( 0, 0, 0, 100 );

    m_painter.drawPixmap( 0, 0, m_loadPixmap );
    m_painter.fillRect( m_loadPixmap.rect(), shadowColor );

    if ( m_isMousePress )
    {
        m_selectedRect = getRect( m_beginPoint, m_endPoint );

        m_capturePixmap = m_loadPixmap.copy( m_selectedRect );
        m_painter.drawPixmap( m_selectedRect.topLeft(), m_capturePixmap );

        m_painter.setPen( QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap) );
        m_painter.drawRect( m_selectedRect );
    }

    m_painter.end();

    QWidget::paintEvent( event );

    return;
}


void CaptureScreen::keyPressEvent( QKeyEvent *event )
{
    if ( Qt::Key_Escape == event->key() )
    {
        close();
    }

    if ( (Qt::Key_Return == event->key()) || (Qt::Key_Enter == event->key()) )
    {
        emit completeCature( m_capturePixmap );
        close();
    }

    QWidget::keyPressEvent( event );

    return;
}


QRect CaptureScreen::getRect( const QPoint &beginPoint, const QPoint &endPoint )
{
    int x = 0, y = 0, width = 0, height = 0;

    width = qAbs( beginPoint.x() - endPoint.x() );
    height = qAbs( beginPoint.y() - endPoint.y() );

    x = ( beginPoint.x() < endPoint.x() ) ? beginPoint.x() : endPoint.x();
    y = ( beginPoint.y() < endPoint.y() ) ? beginPoint.y() : endPoint.y();

    QRect selectedRect = QRect( x, y, width, height );

    if ( 0 == selectedRect.width() )
    {
        selectedRect.setWidth( 1 );
    }

    if ( 0 == selectedRect.height() )
    {
        selectedRect.setHeight( 1 );
    }

    return selectedRect;
}
