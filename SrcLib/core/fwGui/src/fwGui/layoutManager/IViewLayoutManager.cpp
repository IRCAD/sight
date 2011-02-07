/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IViewLayoutManagerBase.cpp
 * @brief This file defines the implementation of the base class for managing a layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/foreach.hpp>

#include "fwGui/layoutManager/IViewLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

IViewLayoutManager::IViewLayoutManager()
{}

//-----------------------------------------------------------------------------

IViewLayoutManager::~IViewLayoutManager()
{}

//-----------------------------------------------------------------------------

void IViewLayoutManager::destroySubViews()
{
    BOOST_FOREACH( ::fwGui::container::fwContainer::sptr container, m_subViews)
    {
        container->destroyContainer();
    }
    m_subViews.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwContainer::sptr > IViewLayoutManager::getSubViews()
{
    return this->m_subViews;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



