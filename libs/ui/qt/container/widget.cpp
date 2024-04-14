/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/container/widget.hpp"

#include <core/macros.hpp>

#include <QDockWidget>
#include <QLayout>
#include <QList>
#include <QMetaObject>
#include <QWidget>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

widget::~widget() noexcept
{
    SIGHT_ASSERT(
        "Error during destruction : The qt container included in this class is still allocated, please call destroyContainer() before.",
        m_container == nullptr
    );
}

//-----------------------------------------------------------------------------

void widget::set_layout(QLayout* const _layout)
{
    SIGHT_ASSERT("The container must be initialized before invoking setLayout().", m_container);

    // Recursively delete all children
    QLayout* old_layout = m_container->layout();

    if(nullptr != old_layout)
    {
        this->clean();

        // Since we will set a new layout, delete the old one as stated in Qt documentation
        delete old_layout;
        old_layout = nullptr;
    }

    // Assign the new layout manager
    m_container->setLayout(_layout);
}

//-----------------------------------------------------------------------------

void widget::clean()
{
    SIGHT_ASSERT("The container must be initialized before invoking clean().", m_container);

    // Recursively delete all children
    QLayout* old_layout = m_container->layout();

    if(nullptr != old_layout)
    {
        // This block layouting when there is a lot of child
        m_container->setUpdatesEnabled(false);

        for(QLayoutItem* child = old_layout->takeAt(0) ; nullptr != child ; child = old_layout->takeAt(0))
        {
            delete child;
        }

        // Restore update
        m_container->setUpdatesEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void widget::destroy_container()
{
    SIGHT_ASSERT("The container must be initialized before invoking destroyContainer().", m_container);

    if(m_container != nullptr)
    {
        m_container->deleteLater();
        m_container.clear();
    }
}

//-----------------------------------------------------------------------------

void widget::set_qt_container(QWidget* _container)
{
    m_container = _container;
}

//-----------------------------------------------------------------------------

QWidget* widget::get_qt_container() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

bool widget::is_shown_on_screen()
{
    SIGHT_ASSERT("The container must be initialized before invoking isShownOnScreen().", m_container);
    return m_container->isVisible();
}

//-----------------------------------------------------------------------------

void widget::set_visible(bool _is_visible)
{
    SIGHT_ASSERT("The container must be initialized before invoking setVisible().", m_container);

    QWidget* parent = m_container->parentWidget();
    auto* dock      = qobject_cast<QDockWidget*>(parent);

    if(dock != nullptr)
    {
        dock->setVisible(_is_visible);
    }

    m_container->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void widget::set_enabled(bool _is_enabled)
{
    SIGHT_ASSERT("The container must be initialized before invoking setEnabled().", m_container);

    QWidget* parent = m_container->parentWidget();
    auto* dock      = qobject_cast<QDockWidget*>(parent);

    if(dock != nullptr)
    {
        dock->setEnabled(_is_enabled);
    }

    m_container->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::container
