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

#include "ui/qt/container/widget.hpp"

#include <core/macros.hpp>

#include <QDockWidget>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

widget::~widget() noexcept
{
    SIGHT_ASSERT(
        "destroy_container() has not been called before destruction.",
        m_container == nullptr && m_root == nullptr
    );
}

//-----------------------------------------------------------------------------

void widget::set_layout(QLayout* const _layout)
{
    SIGHT_ASSERT("The container must be initialized before invoking set_layout().", m_container);

    // Since we will set a new layout, delete the old one as stated in Qt documentation
    if(auto* const old_layout = m_container->layout(); old_layout != nullptr)
    {
        // Delete all child layout items
        this->clean();

        // Old layout can now be deleted
        delete old_layout;
    }

    // Assign the new layout manager
    m_container->setLayout(_layout);
}

//-----------------------------------------------------------------------------

void widget::clean()
{
    SIGHT_ASSERT("The container must be initialized before invoking clean().", m_container);

    // Delete all children
    if(auto* const layout = m_container->layout(); layout != nullptr)
    {
        // This block layouting when there is a lot of child
        m_container->setUpdatesEnabled(false);

        for(QLayoutItem* child = nullptr ; (child = layout->takeAt(0)) != nullptr ; )
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
    SIGHT_ASSERT("The container must be initialized before invoking destroy_container().", m_container);

    if(auto* const root = get_qt_root(); root != nullptr)
    {
        // Should also delete the container, since it should be a child of the root
        root->deleteLater();

        m_root.clear();
        m_container.clear();
    }
}

//-----------------------------------------------------------------------------

bool widget::is_shown_on_screen()
{
    SIGHT_ASSERT("The container must be initialized before invoking is_shown_on_screen().", m_container);

    if(const auto* const root = get_qt_root(); root != nullptr)
    {
        return root->isVisible();
    }

    return false;
}

//-----------------------------------------------------------------------------

void widget::set_visible(bool _is_visible)
{
    SIGHT_ASSERT("The container must be initialized before invoking set_visible().", m_container);

    if(auto* const root = get_qt_root(); root != nullptr)
    {
        if(auto* const dock = qobject_cast<QDockWidget*>(root->parentWidget()); dock != nullptr)
        {
            dock->setVisible(_is_visible);
        }

        root->setVisible(_is_visible);
    }
}

//-----------------------------------------------------------------------------

void widget::set_enabled(bool _is_enabled)
{
    SIGHT_ASSERT("The container must be initialized before invoking setEnabled().", m_container);

    if(auto* const root = get_qt_root(); root != nullptr)
    {
        if(auto* const dock = qobject_cast<QDockWidget*>(root->parentWidget()); dock != nullptr)
        {
            dock->setEnabled(_is_enabled);
        }

        root->setEnabled(_is_enabled);
    }
}

} // namespace sight::ui::qt::container
