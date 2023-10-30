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

class UI_QT_CLASS_API_QT range_slider : public QWidget
{
Q_OBJECT

public:

    class paintable
    {
    public:

        paintable(QWidget* _w) :
            m_widget(_w)
        {
        }

        virtual ~paintable()
        = default;

        virtual void draw(QPainter&, bool _enabled           = true) = 0;
        [[nodiscard]] virtual bool pick(const QPoint&) const = 0;

    protected:

        //------------------------------------------------------------------------------

        [[nodiscard]] QSize drawing_area() const
        {
            return m_widget->size();
        }

        QPointer<QWidget> m_widget;
    };

    UI_QT_API_QT range_slider(QWidget* _parent = nullptr);
    UI_QT_API_QT ~range_slider() override;

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize sizeHint() const override
    {
        return {100, 20};
    }

    //------------------------------------------------------------------------------

    void set_minimum_min_max_delta(double _d)
    {
        m_minimum_min_max_delta = _d;
    }

public Q_SLOTS:

    UI_QT_API_QT void set_pos(double _min, double _max);

Q_SIGNALS:

    void slider_range_changed(double _min, double _max);
    void slider_range_edited(double _min, double _max);

protected:

    void move(int _delta);
    bool moved_to(double _min, double _max);

    void paintEvent(QPaintEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;
    void resizeEvent(QResizeEvent* _event) override;

    paintable* m_min_handle;
    paintable* m_max_handle;
    paintable* m_window;

    paintable* m_current {nullptr};

    int m_handle_size {11};

    QPoint m_press_pos;
    int m_press_min {};
    int m_press_max {};

    double m_min_value {0.};
    double m_max_value {1.};

    double m_minimum_min_max_delta {0.};
    bool m_allow_min_greater_than_max {true};
    bool m_emit_range_changed {};
};

} // namespace sight::ui::qt::widget
