/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_QRANGESLIDER_HPP_
#define _FWGUIQT_QRANGESLIDER_HPP_

#include <QWidget>
#include <QPainter>
#include <QPointer>
#include <QDebug>

#include "fwGuiQt/config.hpp"


namespace fwGuiQt
{

namespace widget
{

class FWGUIQT_CLASS_API QRangeSlider : public QWidget
{
    Q_OBJECT

public:
    class Paintable
    {
    public:
        Paintable(QWidget *w)
        {
            m_widget = w;
        }
        virtual ~Paintable(){}

        virtual void draw(QPainter &, bool enabled = true) = 0;
        virtual bool pick(const QPoint&) const = 0;

    protected:
        QSize drawingArea() const {return m_widget->size();}
        QPointer<QWidget> m_widget;
    };



    FWGUIQT_API QRangeSlider(QWidget *parent = NULL);
    FWGUIQT_API virtual ~QRangeSlider();

    QSize sizeHint() const {return QSize(100,20);};

    void setMinimumMinMaxDelta(double d){ m_minimumMinMaxDelta = d;}

public slots:
    FWGUIQT_API void setPos(double _min, double _max);

signals:
    void sliderRangeChanged ( double min, double max);
    void sliderRangeEdited ( double min, double max);

protected:


    void move(int delta);
    bool movedTo(double _min, double _max);

    virtual void paintEvent ( QPaintEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void wheelEvent ( QWheelEvent * event );
    virtual void resizeEvent ( QResizeEvent * event );

    Paintable *m_minHandle;
    Paintable *m_maxHandle;
    Paintable *m_window;

    Paintable *m_current;

    int m_handleSize;

    QPoint m_pressPos;
    int m_pressMin;
    int m_pressMax;

    double m_minValue;
    double m_maxValue;

    double m_minimumMinMaxDelta;
    bool m_allowMinGreaterThanMax;
    bool m_emitRangeChanged;

};


} // namespace widget
} // namespace fwGuiQt

#endif // _FWGUIQT_QRANGESLIDER_HPP_
