/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_
#define _FWGUIWX_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/LineLayoutManagerBase.hpp>

#include "fwGuiWx/container/WxContainer.hpp"
#include "fwGuiWx/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the line layout manager.
 * @class   LineLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API LineLayoutManager : public ::fwGui::layoutManager::LineLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LineLayoutManager)(::fwGui::layoutManager::LineLayoutManagerBase), (()), new LineLayoutManager );

    /// Constructor. Do nothing.
    FWGUIWX_API LineLayoutManager();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~LineLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIWX_API virtual void createLayout( ::fwGui::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIWX_API virtual void destroyLayout();

private:
    ::fwGuiWx::container::WxContainer::sptr m_parentContainer;
};

} // namespace fwGui

#endif /*_FWGUIWX_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_*/


