/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/ui/qt/config.hpp>

#include <core/base.hpp>
#include <core/object.hpp>

#include <ui/__/container/widget.hpp>

#include <QLayout>
#include <QPointer>
#include <QWidget>

namespace sight::ui::qt::container
{

/**
 * @brief   Defines the QWidget container.
 *
 */

class SIGHT_UI_QT_CLASS_API widget : public ui::container::widget
{
public:

    SIGHT_DECLARE_CLASS(widget, ui::container::widget, ui::factory::make<widget>);

    SIGHT_UI_QT_API ~widget() noexcept override;

    inline void set_qt_container(QWidget* _container, QWidget* _root = nullptr);
    inline QWidget* get_qt_container() const;
    inline QWidget* get_qt_root() const;

    /**
     * @brief Assign a layout to the container widget. Previous layout and its children are deleted and the container
     *  widget take the ownership of the new layout
     *
     * @param _layout   The layout to set on the container widget
     */
    SIGHT_UI_QT_API virtual void set_layout(QLayout* _layout);

    SIGHT_UI_QT_API void clean() override;
    SIGHT_UI_QT_API void destroy_container() override;
    SIGHT_UI_QT_API bool is_shown_on_screen() override;

    SIGHT_UI_QT_API void set_visible(bool _is_visible) override;
    SIGHT_UI_QT_API void set_enabled(bool _is_enabled) override;

private:

    QPointer<QWidget> m_container;
    QPointer<QWidget> m_root;
};

//-----------------------------------------------------------------------------

inline void widget::set_qt_container(QWidget* _container, QWidget* _root)
{
    m_container = _container;
    m_root      = _root;
}

//-----------------------------------------------------------------------------

inline QWidget* widget::get_qt_container() const
{
    return m_container;
}

//------------------------------------------------------------------------------

inline QWidget* widget::get_qt_root() const
{
    return m_root ? m_root : m_container;
}

} // namespace sight::ui::qt::container
