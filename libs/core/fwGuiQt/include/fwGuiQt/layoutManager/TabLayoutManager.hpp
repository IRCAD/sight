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
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/TabLayoutManagerBase.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace fwGui
{

/**
 * @brief   Defines the tab layout manager.
 */
class FWGUIQT_CLASS_API TabLayoutManager : public ::fwGui::layoutManager::TabLayoutManagerBase
{

public:

    fwCoreClassMacro(TabLayoutManager, ::fwGui::layoutManager::TabLayoutManagerBase,
                     ::fwGui::factory::New< TabLayoutManager >);

    FWGUIQT_API TabLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~TabLayoutManager();

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

private:

    ::fwGuiQt::container::QtContainer::sptr m_parentContainer;

    // Layout manager of this view
    QPointer<QTabWidget> m_tabWidget;
};

} // namespace fwGui
