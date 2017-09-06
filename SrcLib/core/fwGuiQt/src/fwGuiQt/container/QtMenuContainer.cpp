/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMenu>
#include "fwGuiQt/container/QtMenuContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtMenuContainer::QtMenuContainer(::fwGui::GuiBaseObject::Key key) noexcept
{
}

//-----------------------------------------------------------------------------

QtMenuContainer::~QtMenuContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QtMenuContainer::clean()
{
    SLM_ASSERT("The Qt menu is not yet initialized, cleaning is thus impossible", m_menu);
    m_menu->clear();
}

//-----------------------------------------------------------------------------

void QtMenuContainer::destroyContainer()
{
    SLM_ASSERT("The Qt menu not yet initialized, cleaning is thus impossible", m_menu);
    delete m_menu;
}

//-----------------------------------------------------------------------------

void QtMenuContainer::setQtMenu(QMenu *menu)
{
    this->m_menu = menu;
}

//-----------------------------------------------------------------------------

QMenu* QtMenuContainer::getQtMenu()
{
    return this->m_menu;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
