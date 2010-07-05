/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/layoutManager/ILayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ILayoutManager::ILayoutManager()
{}

//-----------------------------------------------------------------------------

ILayoutManager::~ILayoutManager()
{}

//-----------------------------------------------------------------------------

void ILayoutManager::destroySubViews()
{
    BOOST_FOREACH( ::fwGui::fwContainer::sptr container, m_subViews)
    {
        container->destroyContainer();
    }
    m_subViews.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::fwContainer::sptr > ILayoutManager::getSubViews()
{
    return this->m_subViews;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



