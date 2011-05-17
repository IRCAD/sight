#include <cassert>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>


#include "fwGuiQt/widget/QRangeSlider.hpp"


namespace fwGuiQt
{

namespace widget
{

template <typename T>
struct Castable
{
    template <typename U>
    static T* safeCast(U *p)
    {
        T* casted = dynamic_cast<T*>(p);
        return casted;
    }
};



class Handle: public QRangeSlider::Paintable, public Castable<Handle>
{
public:
    Handle(QWidget *w)
        : Paintable(w),
          m_pen(Qt::gray),
          m_brush(Qt::lightGray)
    {
        m_pos = 0;
        m_width = 13;
        m_verticalPadding = 0.2;
        m_tolerance = std::max(0, 10 - m_width);
    }

    virtual void draw(QPainter &painter, bool /*enabled*/)
    {
        int height = drawingArea().height()-1;
        int top = height * m_verticalPadding;
        int handleHeight = height - 2*top;
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(m_pen);
        painter.setBrush(m_brush);
        painter.drawRect(m_pos - halfWidth(), top, m_width, handleHeight);
    }

    virtual bool pick(const QPoint &point) const
    {
        bool picked = false;

        int height = drawingArea().height()-1;
        int top = height * m_verticalPadding;
        int handleHeight = height - 2*top;

        if ( abs(point.x() - m_pos) <= (halfWidth() + m_tolerance)
            && (top+handleHeight) >= point.y() && point.y() >= top )
        {
            picked = true;
        }
        return picked;
    }

    int setHandleSize(int size)
    {
        //handle size should be odd
        m_width = size + ((size+1)%2);
        return m_width;
    }

    int halfWidth() const
    {
        return m_width/2;
    }

    int setPos(const int &pos)
    {
        int p = pos;

        p = std::max(p, xPosMin());
        p = std::min(p, xPosMax());

        m_pos = p;

        return p;
    }

    int pos()
    {
        return m_pos;
    }

    int xPosMin()
    {
        return halfWidth();
    }

    int xPosMax()
    {
        return drawingArea().width() - halfWidth() - 1;
    }

    double toFloatingPos(int p)
    {
        int posMin = xPosMin();
        int posMax = xPosMax();
        return ( (double) (p - posMin) / (double)(posMax - posMin) );
    }

    int fromFloatingPos(double p)
    {
        assert( 0. <= p && p <= 1.);
        int posMin = xPosMin();
        int extend = (xPosMax()) - posMin;

        return posMin + (int) (p*extend);
    }


    double floatingPos()
    {
        return toFloatingPos( pos() );
    }

    void setFloatingPos(double pos)
    {
        setPos(fromFloatingPos(pos));
    }

protected:
    int m_pos;
    int m_tolerance;
    int m_width;
    double m_verticalPadding;

    QPen   m_pen;
    QBrush m_brush;
};


class Window: public QRangeSlider::Paintable, public Castable<Window>
{
public:
    Window(QWidget *w)
        : Paintable(w),
        m_pen(Qt::darkBlue),
        m_brush(Qt::cyan),
        m_reversePen(Qt::darkYellow),
        m_reverseBrush(Qt::yellow)
    {
        m_left = 0;
        m_right = 0;
    }

    int width() const
    {
        return m_right-m_left;
    }

    virtual void draw(QPainter &painter, bool enabled)
    {
        int w = this->width();
        QPen   pen;
        QBrush brush;

        if(enabled)
        {
            if ( w < 0 )
            {
                pen = m_reversePen;
                brush = m_reverseBrush;
            }
            else
            {
                pen = m_pen;
                brush = m_brush;
            }
        }
        else
        {
                pen = QPen(Qt::lightGray);
                brush = QBrush(Qt::lightGray);
        }

        painter.setPen(pen);
        painter.setBrush(brush);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawRect(m_left, 0, w, drawingArea().height()-1);
    }

    virtual bool pick(const QPoint &point) const
    {
        bool picked = false;
        int min = std::min(m_left, m_right);
        int max = std::max(m_left, m_right);
        picked = min <= point.x() && point.x() <= max ;
        return picked;
    }

    void setPos(const int &left, const int &right)
    {
        m_left = left;
        m_right = right;
    }

protected:
    int m_left;
    int m_right;

    QPen   m_pen;
    QBrush m_brush;
    QPen   m_reversePen;
    QBrush m_reverseBrush;

};



//-----------------------------------------------------------------------------

QRangeSlider::QRangeSlider(QWidget *parent)
    : QWidget(parent)
{
    m_minValue = 0.;
    m_maxValue = 1.;
    m_allowMinGreaterThanMax = false;
    m_minimumMinMaxDelta = 0.;
    m_handleSize = 11;

    m_current = NULL;

    Handle *minh = new Handle(this);
    Handle *maxh = new Handle(this);
    minh->setHandleSize(m_handleSize);
    maxh->setHandleSize(m_handleSize);

    m_minHandle = minh;
    m_maxHandle = maxh;
    m_window    = new Window(this);

    this->setPos(m_minValue, m_maxValue);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}


QRangeSlider::~QRangeSlider()
{
     delete m_minHandle;
     delete m_maxHandle;
     delete m_window;
}

void QRangeSlider::setPos(double _min, double _max)
{
    Handle *minHandle = Handle::safeCast(m_minHandle);
    Handle *maxHandle = Handle::safeCast(m_maxHandle);
    Window *window    = Window::safeCast(m_window);

    assert(minHandle && maxHandle && window);

    minHandle->setFloatingPos(_min);
    maxHandle->setFloatingPos(_max);

    int min, max;
    min = minHandle->pos();
    max = maxHandle->pos();
    window->setPos(min, max);

    this->movedTo(_min, _max);
    this->update();
}

void QRangeSlider::move(int delta)
{
    Handle *minHandle = Handle::safeCast(m_minHandle);
    Handle *maxHandle = Handle::safeCast(m_maxHandle);
    Window *window    = Window::safeCast(m_window);

    assert(minHandle && maxHandle && window);

    int low, high, width, dir;
    dir = ((minHandle->pos() < maxHandle->pos()) ? 1 : -1);
    bool movingRight =  (delta < 0) ;

    low  = minHandle->pos();
    high = maxHandle->pos();
    width = high - low;

    if(  (movingRight  && dir < 0)
        || (!movingRight && dir > 0 ) )
    {
        low  = minHandle->setPos(low-delta);
        high = low + width;
        maxHandle->setPos(high);
    }
    else
    {
        high = maxHandle->setPos(high-delta);
        low  = high - width;
        minHandle->setPos(low);
    }

    window->setPos(low, high);
}

bool QRangeSlider::movedTo(double _min, double _max)
{
    bool changed = m_minValue != _min || m_maxValue != _max ;
    if (changed)
    {
        m_minValue = _min;
        m_maxValue = _max;
        emit sliderRangeChanged( m_minValue, m_maxValue );
    }
    return changed;
}

void QRangeSlider::paintEvent ( QPaintEvent * /*event*/ )
{
    bool enabled = this->isEnabled();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    rect.setLeft(rect.left() + m_handleSize/2);
    rect.setRight(rect.right() - m_handleSize/2);
    painter.fillRect(rect, QApplication::palette().base());

    painter.setBrush(Qt::cyan);
    m_window->draw(painter, enabled);

    painter.setBrush(Qt::gray);
    painter.setPen(Qt::darkGreen);
    m_minHandle->draw(painter, enabled);

    painter.setPen(Qt::darkRed);
    m_maxHandle->draw(painter, enabled);

}

void QRangeSlider::mouseMoveEvent ( QMouseEvent * event )
{
    if (m_current)
    {

        Handle *minHandle = Handle::safeCast(m_minHandle);
        Handle *maxHandle = Handle::safeCast(m_maxHandle);
        Window *window    = Window::safeCast(m_window);
        Handle *currentHandle;


        if( (currentHandle = Handle::safeCast(m_current)) )
        {
            int oldPos = currentHandle->pos();
            int newPos = event->pos().x();
            currentHandle->setPos(newPos);

            if(!m_allowMinGreaterThanMax && minHandle->floatingPos() + m_minimumMinMaxDelta >= maxHandle->floatingPos() )
            {
                currentHandle->setPos(oldPos);
            }
            window->setPos(minHandle->pos(), maxHandle->pos());
        }
        else if( Window::safeCast(m_current) )
        {
            QPoint delta = m_pressPos - event->pos();

            minHandle->setPos(m_pressMin);
            maxHandle->setPos(m_pressMax);
            window->setPos(minHandle->pos(), maxHandle->pos());
            this->move(delta.x());
        }

        double min = minHandle->floatingPos();
        double max = maxHandle->floatingPos();
        if( this->movedTo(min, max) )
        {
            emit sliderRangeEdited ( min, max );
        }
        this->update();
    }
}

void QRangeSlider::mousePressEvent ( QMouseEvent * event )
{
    Handle *minHandle  = Handle::safeCast(m_minHandle);
    Handle *maxHandle = Handle::safeCast(m_maxHandle);
//    Window *window     = Window::safeCast(m_window);

    m_pressPos  = event->pos();
    m_pressMin  = minHandle->pos();
    m_pressMax = maxHandle->pos();

    if(m_minHandle->pick(m_pressPos))
    {
        m_current = m_minHandle;
    }
    else if(m_maxHandle->pick(m_pressPos))
    {
        m_current = m_maxHandle;
    }
    else if(m_window->pick(m_pressPos))
    {
        m_current = m_window;
    }
}

void QRangeSlider::mouseReleaseEvent ( QMouseEvent * /*event*/)
{
    m_current = NULL;
}


void QRangeSlider::wheelEvent ( QWheelEvent * event )
{
    Handle *minHandle  = Handle::safeCast(m_minHandle);
    Handle *maxHandle = Handle::safeCast(m_maxHandle);
    Window *window     = Window::safeCast(m_window);

    int delta = this->size().width()/( ((double) event->delta())/4. );
    int low  = minHandle->pos();
    int high = maxHandle->pos();

    if(event->orientation() == Qt::Vertical)
    {
        if(!m_allowMinGreaterThanMax)
        {
            int diff = (high - low);
            int minDiff = minHandle->fromFloatingPos(m_minimumMinMaxDelta);
            delta = std::max(delta,  - (diff - minDiff)/2);
        }

        low  = minHandle->setPos(low -  delta);
        high = maxHandle->setPos(high + delta);
        window->setPos(low, high);
    }
    else
    {
        this->move(delta);
    }

    double min = minHandle->floatingPos();
    double max = maxHandle->floatingPos();
    if( this->movedTo(min, max) )
    {
        emit sliderRangeEdited ( min, max );
    }
    this->update();
}

void QRangeSlider::resizeEvent ( QResizeEvent * event )
{
    this->setPos(m_minValue, m_maxValue);
}

} // namespace widget
} // namespace fwGuiQt
