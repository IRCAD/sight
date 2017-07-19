/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QAction>
#include "fwGuiQt/container/QtMenuItemContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtMenuItemContainer::QtMenuItemContainer(::fwGui::GuiBaseObject::Key key) noexcept : m_menuItem(0)
{
}

//-----------------------------------------------------------------------------

QtMenuItemContainer::~QtMenuItemContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::clean()
{
    SLM_ASSERT("The qtMenuItem is not yet initialized, cleaning is thus impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::destroyContainer()
{
    SLM_ASSERT("The qtMenuItem is not yet initialized, cleaning is thus impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::setQtMenuItem(QAction* menuItem)
{
    this->m_menuItem = menuItem;
}

//-----------------------------------------------------------------------------

QAction* QtMenuItemContainer::getQtMenuItem()
{
    return this->m_menuItem;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
