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

#pragma once

#include <ui/qt/config.hpp>

#include <QGestureRecognizer>
#include <QPanGesture>
#include <QPointF>

namespace sight::ui::qt::gestures
{

/// A class representing a pan gesture
class PanGesture : public QGesture
{
public:

    /**
     * These methods are provided to be compatible with QPanGesture. @see https://doc.qt.io/qt-5/qpangesture.html
     * @{
     */
    UI_QT_API qreal acceleration() const;
    void setAcceleration(qreal value);
    UI_QT_API QPointF delta() const;
    UI_QT_API QPointF lastOffset() const;
    void setLastOffset(const QPointF& value);
    UI_QT_API QPointF offset() const;
    void setOffset(const QPointF& value);
    /// @}

    /// The current position of the finger
    UI_QT_API QPointF position() const;
    /// Set the current position of the finger (mostly useful for QPanGestureRecognizer)
    void setPosition(const QPointF& pos);

    ~PanGesture() override = default;

private:

    QPanGesture m_qPanGesture;
    QPointF m_position;
};

class UI_QT_CLASS_API QPanGestureRecognizer : public QGestureRecognizer
{
public:

    explicit QPanGestureRecognizer(int pointCount = 2) :
        m_pointCount(pointCount)
    {
    }

    UI_QT_API QGesture* create(QObject* target) override;
    UI_QT_API QGestureRecognizer::Result recognize(QGesture* state, QObject* watched, QEvent* event) override;
    UI_QT_API void reset(QGesture* state) override;

    //------------------------------------------------------------------------------

    template<int N = 1>
    static Qt::GestureType get()
    {
        static auto res = static_cast<Qt::GestureType>(0);
        if(res == 0)
        {
            res = QGestureRecognizer::registerRecognizer(new QPanGestureRecognizer(N));
        }

        return res;
    }

private:

    const int m_pointCount;
};

} // namespace sight::ui::qt::gestures
