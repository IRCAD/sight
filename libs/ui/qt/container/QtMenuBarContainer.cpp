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

#include "ui/qt/container/QtMenuBarContainer.hpp"

#include <QMenuBar>

namespace sight::ui::qt
{
namespace container
{

//-----------------------------------------------------------------------------

QtMenuBarContainer::QtMenuBarContainer(ui::base::GuiBaseObject::Key key) noexcept :
    m_menuBar(0)
{
}

//-----------------------------------------------------------------------------

QtMenuBarContainer::~QtMenuBarContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::clean()
{
    SLM_ASSERT("The Qt MenuBar is not yet initialized, cleaning is thus impossible", m_menuBar);

    m_menuBar->clear();

    QList<QMenu*> list = m_menuBar->findChildren<QMenu*>();
    Q_FOREACH (QMenu* m, list)
    {
        m->hide();
        delete m;
    }
}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::destroyContainer()
{
    SLM_ASSERT("The Qt MenuBar is not yet initialized", m_menuBar);

    QList<QMenu*> allMenu = m_menuBar->findChildren<QMenu*>();
    SLM_ASSERT("MenuBar container must be empty ( " << allMenu.count() << " menus).", allMenu.empty());

    m_menuBar->hide();
    delete m_menuBar;
}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::setQtMenuBar(QMenuBar* menuBar)
{
    this->m_menuBar = menuBar;
}

//-----------------------------------------------------------------------------

QMenuBar* QtMenuBarContainer::getQtMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace sight::ui::qt
