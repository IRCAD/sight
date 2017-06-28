/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/layoutManager/IViewLayoutManager.cpp
 * @brief This file defines the implementation of the base class for managing a layout geometry.
 *
 */

#include "fwGui/layoutManager/IViewLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

IViewLayoutManager::IViewLayoutManager()
{
}

//-----------------------------------------------------------------------------

IViewLayoutManager::~IViewLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IViewLayoutManager::destroySubViews()
{
    for( ::fwGui::container::fwContainer::sptr container :  m_subViews)
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

