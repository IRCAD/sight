/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/builder/MenuBarBuilder.hpp"

#include "ui/qt/App.hpp"
#include "ui/qt/container/QtContainer.hpp"
#include "ui/qt/container/QtMenuBarContainer.hpp"

#include <data/tools/Color.hpp>

#include <ui/base/registry/macros.hpp>

#include <QMainWindow>
#include <QMenuBar>

fwGuiRegisterMacro(
    ::sight::ui::base::builder::MenuBarBuilder,
    ::sight::ui::base::builder::IMenuBarBuilder::REGISTRY_KEY
);

namespace sight::ui::base
{

namespace builder
{

//-----------------------------------------------------------------------------

MenuBarBuilder::MenuBarBuilder(ui::base::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuBarBuilder::~MenuBarBuilder()
{
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::createMenuBar(ui::base::container::fwContainer::sptr parent)
{
    m_parent = ui::qt::container::QtContainer::dynamicCast(parent);
    SIGHT_ASSERT("The parent container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());
    if(!window)
    {
        window = qobject_cast<QMainWindow*>(m_parent->getQtContainer()->parent());
    }

    SIGHT_ASSERT("The parent container must be a QMainWindow", window);
    if(window)
    {
        ui::qt::container::QtMenuBarContainer::sptr menuBarContainer =
            ui::qt::container::QtMenuBarContainer::New();
        QMenuBar* menuBar = new QMenuBar(0);
        menuBarContainer->setQtMenuBar(menuBar);
        window->setMenuBar(menuBar);
        m_menuBar = menuBarContainer;

        if(!m_backgroundColor.empty())
        {
            std::uint8_t rgba[4];
            data::tools::Color::hexaStringToRGBA(m_backgroundColor, rgba);
            std::stringstream ss;
            ss << "QMenuBar, QMenu { background-color: rgba(" << static_cast<short>(rgba[0]) << ','
            << static_cast<short>(rgba[1]) << ','
            << static_cast<short>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.f) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            menuBar->setStyleSheet(qApp->styleSheet() + style);
        }
    }
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::destroyMenuBar()
{
    SIGHT_ASSERT("The menu is not initialized", m_menuBar);
    SIGHT_ASSERT("The parent container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());
    if(!window)
    {
        window = qobject_cast<QMainWindow*>(m_parent->getQtContainer()->parent());
    }

    SIGHT_ASSERT("The parent container must be a QMainWindow", window);
    if(window)
    {
        window->setMenuBar(NULL);
    }

    m_menuBar->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder

} // namespace sight::ui::base
