/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

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

    fwCoreClassMacro(ToolboxLayoutManager, ::fwGui::layoutManager::ToolboxLayoutManagerBase,
                     ::fwGui::factory::New< ToolboxLayoutManager >);

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
