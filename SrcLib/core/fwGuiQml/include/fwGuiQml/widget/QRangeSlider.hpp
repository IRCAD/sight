/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwGuiQml/config.hpp"

#include <QDebug>
#include <QPainter>
#include <QPointer>
#include <QWidget>

namespace fwGuiQml
{

namespace widget
{

class FWGUIQML_CLASS_API QRangeSlider : public QWidget
{
Q_OBJECT

public:
    class Paintable
    {
    public:
        Paintable(QWidget* w)
        {
            m_widget = w;
        }
        virtual ~Paintable()
        {
        }

        virtual void draw(QPainter&, bool enabled = true) = 0;
        virtual bool pick(const QPoint&) const = 0;

    protected:
        //------------------------------------------------------------------------------

        QSize drawingArea() const
        {
            return m_widget->size();
        }
        QPointer<QWidget> m_widget;
    };

    FWGUIQML_API QRangeSlider(QWidget* parent = NULL);
    FWGUIQML_API virtual ~QRangeSlider();

    //------------------------------------------------------------------------------

    QSize sizeHint() const
    {
        return QSize(100, 20);
    }

    //------------------------------------------------------------------------------

    void setMinimumMinMaxDelta(double d)
    {
        m_minimumMinMaxDelta = d;
    }

public Q_SLOTS:
    FWGUIQML_API void setPos(double _min, double _max);

Q_SIGNALS:
    void sliderRangeChanged ( double min, double max);
    void sliderRangeEdited ( double min, double max);

protected:

    void move(int delta);
    bool movedTo(double _min, double _max);

    virtual void paintEvent ( QPaintEvent* event );
    virtual void mouseMoveEvent ( QMouseEvent* event );
    virtual void mousePressEvent ( QMouseEvent* event );
    virtual void mouseReleaseEvent ( QMouseEvent* event );
    virtual void wheelEvent ( QWheelEvent* event );
    virtual void resizeEvent ( QResizeEvent* event );

    Paintable* m_minHandle;
    Paintable* m_maxHandle;
    Paintable* m_window;

    Paintable* m_current;

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
} // namespace fwGuiQml
