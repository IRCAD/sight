/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

// Mostly copied from "QPanGestureRecognizer" from Qt5.
// QPanGesture from Qt5 requires two touch points to work. This version only requires one.

#include "QPanGestureRecognizer.hpp"

#include <QAbstractScrollArea>
#include <QObject>
#include <QPanGesture>
#include <QWidget>

namespace sight::ui::qt::gestures
{

//------------------------------------------------------------------------------

qreal PanGesture::acceleration() const
{
    return m_qPanGesture.acceleration();
}

//------------------------------------------------------------------------------

void PanGesture::setAcceleration(qreal value)
{
    m_qPanGesture.setAcceleration(value);
}

//------------------------------------------------------------------------------

QPointF PanGesture::delta() const
{
    return m_qPanGesture.delta();
}

//------------------------------------------------------------------------------

QPointF PanGesture::lastOffset() const
{
    return m_qPanGesture.lastOffset();
}

//------------------------------------------------------------------------------

void PanGesture::setLastOffset(const QPointF& value)
{
    m_qPanGesture.setLastOffset(value);
}

//------------------------------------------------------------------------------

QPointF PanGesture::offset() const
{
    return m_qPanGesture.offset();
}

//------------------------------------------------------------------------------

void PanGesture::setOffset(const QPointF& value)
{
    m_qPanGesture.setOffset(value);
}

//------------------------------------------------------------------------------

QPointF PanGesture::position() const
{
    return m_position;
}

//------------------------------------------------------------------------------

void PanGesture::setPosition(const QPointF& pos)
{
    m_position = pos;
}

//------------------------------------------------------------------------------

QGesture* QPanGestureRecognizer::create(QObject* target)
{
    if((target != nullptr) && target->isWidgetType())
    {
#if (defined(Q_OS_MACX) || defined(Q_OS_WIN)) && !defined(QT_NO_NATIVE_GESTURES)
        // for scroll areas on Windows and OS X we want to use native gestures instead
        if(!qobject_cast<QAbstractScrollArea*>(target->parent()))
        {
            static_cast<QWidget*>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
        }
#else
        static_cast<QWidget*>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    }

    return new PanGesture;
}

//------------------------------------------------------------------------------

static QPointF panOffset(const QList<QTouchEvent::TouchPoint>& touchPoints, int maxCount)
{
    QPointF result;
    const int count = qMin(touchPoints.size(), maxCount);
    for(int p = 0 ; p < count ; ++p)
    {
        result += touchPoints.at(p).pos() - touchPoints.at(p).startPos();
    }

    return result / qreal(count);
}

//------------------------------------------------------------------------------

static QPointF center(const QList<QTouchEvent::TouchPoint>& touchPoints)
{
    QPointF result;
    for(const auto& touchPoint : touchPoints)
    {
        result += touchPoint.screenPos();
    }

    return result / qreal(touchPoints.size());
}

//------------------------------------------------------------------------------

QGestureRecognizer::Result QPanGestureRecognizer::recognize(
    QGesture* state,
    QObject* /*watched*/,
    QEvent* event
)
{
    auto* q = static_cast<PanGesture*>(state);

    QGestureRecognizer::Result result = QGestureRecognizer::Ignore;
    switch(event->type())
    {
        case QEvent::TouchBegin:
        {
            const auto* ev = static_cast<const QTouchEvent*>(event);
            result = QGestureRecognizer::MayBeGesture;
            q->setLastOffset(QPointF());
            q->setOffset(QPointF());
            q->setPosition(center(ev->touchPoints()));
            break;
        }

        case QEvent::TouchEnd:
        {
            if(q->state() != Qt::NoGesture)
            {
                const auto* ev = static_cast<const QTouchEvent*>(event);
                if(ev->touchPoints().size() == m_pointCount)
                {
                    q->setLastOffset(q->offset());
                    q->setOffset(panOffset(ev->touchPoints(), m_pointCount));
                    q->setPosition(center(ev->touchPoints()));
                }

                result = QGestureRecognizer::FinishGesture;
            }
            else
            {
                result = QGestureRecognizer::CancelGesture;
            }

            break;
        }

        case QEvent::TouchUpdate:
        {
            const auto* ev = static_cast<const QTouchEvent*>(event);
            if(ev->touchPoints().size() == m_pointCount)
            {
                q->setLastOffset(q->offset());
                q->setOffset(panOffset(ev->touchPoints(), m_pointCount));
                q->setPosition(center(ev->touchPoints()));
                if(q->offset().x() > 10 || q->offset().y() > 10
                   || q->offset().x() < -10 || q->offset().y() < -10)
                {
                    q->setHotSpot(ev->touchPoints().first().startScreenPos());
                    result = QGestureRecognizer::TriggerGesture;
                }
                else
                {
                    result = QGestureRecognizer::MayBeGesture;
                }
            }

            break;
        }

        default:
            break;
    }

    return result;
}

//------------------------------------------------------------------------------

void QPanGestureRecognizer::reset(QGesture* state)
{
    auto* pan = static_cast<PanGesture*>(state);

    pan->setLastOffset(QPointF());
    pan->setOffset(QPointF());
    pan->setAcceleration(0);
    pan->setPosition(QPointF());

    QGestureRecognizer::reset(state);
}

} // namespace sight::ui::qt::gestures
