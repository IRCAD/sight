/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP__
#define __FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/widget/QfwToolbox.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/ToolboxLayoutManagerBase.hpp>

namespace fwGui
{

/**
 * @brief   Defines the toolbox layout manager.
 */
class FWGUIQT_CLASS_API ToolboxLayoutManager : public ::fwGui::layoutManager::ToolboxLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ToolboxLayoutManager)(::fwGui::layoutManager::ToolboxLayoutManagerBase),
                                            (()),
                                            ::fwGui::factory::New< ToolboxLayoutManager > );

    FWGUIQT_API ToolboxLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~ToolboxLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout() override;

};

} // namespace fwGui

#endif /*__FWGUIQT_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGER_HPP__*/

