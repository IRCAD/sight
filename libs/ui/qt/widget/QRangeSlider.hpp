/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/qt/config.hpp"

#include <QDebug>
#include <QPainter>
#include <QPointer>
#include <QWidget>

namespace sight::ui::qt::widget
{

class UI_QT_CLASS_QT_API QRangeSlider : public QWidget
{
Q_OBJECT

public:

    class Paintable
    {
    public:

        Paintable(QWidget* w) :
            m_widget(w)
        {
        }

        virtual ~Paintable()
        = default;

        virtual void draw(QPainter&, bool enabled            = true) = 0;
        [[nodiscard]] virtual bool pick(const QPoint&) const = 0;

    protected:

        //------------------------------------------------------------------------------

        [[nodiscard]] QSize drawingArea() const
        {
            return m_widget->size();
        }

        QPointer<QWidget> m_widget;
    };

    UI_QT_QT_API QRangeSlider(QWidget* parent = nullptr);
    UI_QT_QT_API ~QRangeSlider() override;

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize sizeHint() const override
    {
        return {100, 20};
    }

    //------------------------------------------------------------------------------

    void setMinimumMinMaxDelta(double d)
    {
        m_minimumMinMaxDelta = d;
    }

public Q_SLOTS:

    UI_QT_QT_API void setPos(double _min, double _max);

Q_SIGNALS:

    void sliderRangeChanged(double min, double max);
    void sliderRangeEdited(double min, double max);

protected:

    void move(int delta);
    bool movedTo(double _min, double _max);

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    Paintable* m_minHandle;
    Paintable* m_maxHandle;
    Paintable* m_window;

    Paintable* m_current {nullptr};

    int m_handleSize {11};

    QPoint m_pressPos;
    int m_pressMin {};
    int m_pressMax {};

    double m_minValue {0.};
    double m_maxValue {1.};

    double m_minimumMinMaxDelta {0.};
    bool m_allowMinGreaterThanMax {true};
    bool m_emitRangeChanged {};
};

} // namespace sight::ui::qt::widget
