/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/ToolboxLayoutManagerBase.hpp>

#include "fwGuiQt/widget/QfwToolbox.hpp"
#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the toolbox layout manager.
 * @class   ToolboxLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API ToolboxLayoutManager : public ::fwGui::layoutManager::ToolboxLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ToolboxLayoutManager)(::fwGui::layoutManager::ToolboxLayoutManagerBase), (()), new ToolboxLayoutManager );

    /// Constructor. Do nothing.
    FWGUIQT_API ToolboxLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~ToolboxLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout();

private:
    ::fwGuiQt::container::QtContainer::sptr m_parentContainer;

};

} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP_*/


