/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

QtMenuItemContainer::QtMenuItemContainer() throw() : m_menuItem(0)
{}

//-----------------------------------------------------------------------------

QtMenuItemContainer::~QtMenuItemContainer() throw()
{}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::clean()
{
    SLM_ASSERT("Sorry, qtMenuItem not yet initialized, cleaning impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, qtMenuItem not yet initialized, cleaning impossible", m_menuItem);
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
