/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "ui/qt/builder/menubar.hpp"
#include "ui/qt/container/menubar.hpp"

#include "ui/qt/app.hpp"
#include "ui/qt/container/widget.hpp"

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QMainWindow>
#include <QMenuBar>

SIGHT_REGISTER_GUI(sight::ui::qt::builder::menubar, sight::ui::builder::menubar::REGISTRY_KEY);

namespace sight::ui::qt::builder
{

//-----------------------------------------------------------------------------

void menubar::create_menu_bar(ui::container::widget::sptr _parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);
    auto* window = qobject_cast<QMainWindow*>(m_parent->get_qt_container());
    if(window == nullptr)
    {
        window = qobject_cast<QMainWindow*>(m_parent->get_qt_container()->parent());
    }

    SIGHT_ASSERT("The parent container must be a QMainWindow", window);
    if(window != nullptr)
    {
        auto menu_bar_container = ui::qt::container::menubar::make();
        auto* menu_bar          = new QMenuBar(nullptr);
        menu_bar_container->set_qt_menu_bar(menu_bar);
        window->setMenuBar(menu_bar);
        m_menu_bar = menu_bar_container;

        if(!m_background_color.empty())
        {
            std::array<std::uint8_t, 4> rgba {};
            data::tools::color::hexa_string_to_rgba(m_background_color, rgba);
            std::stringstream ss;
            ss << "QMenuBar, QMenu { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
            << static_cast<std::int16_t>(rgba[1]) << ','
            << static_cast<std::int16_t>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            menu_bar->setStyleSheet(qApp->styleSheet() + style);
        }

        if(!m_qss_class.empty())
        {
            menu_bar->setProperty("class", QString::fromStdString(m_qss_class));
        }
    }
}

//-----------------------------------------------------------------------------

void menubar::destroy_menu_bar()
{
    SIGHT_ASSERT("The menu is not initialized", m_menu_bar);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);
    auto* window = qobject_cast<QMainWindow*>(m_parent->get_qt_container());
    if(window == nullptr)
    {
        window = qobject_cast<QMainWindow*>(m_parent->get_qt_container()->parent());
    }

    SIGHT_ASSERT("The parent container must be a QMainWindow", window);
    if(window != nullptr)
    {
        window->setMenuBar(nullptr);
    }

    m_menu_bar->destroy_container();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
