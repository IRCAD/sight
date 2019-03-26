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

#include "fwGuiQml/config.hpp"
#include "fwGuiQml/container/QmlContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/CardinalLayoutManagerBase.hpp>

#include <QPointer>

class QMainWindow;

namespace fwGui
{

/**
 * @brief   Defines the cardinal layout manager.
 */
class FWGUIQML_CLASS_API CardinalLayoutManager : public ::fwGui::layoutManager::CardinalLayoutManagerBase
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (CardinalLayoutManager)(::fwGui::layoutManager::CardinalLayoutManagerBase),
                                            (()),
                                            ::fwGui::factory::New< CardinalLayoutManager > );

    FWGUIQML_API CardinalLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~CardinalLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    FWGUIQML_API virtual void createLayout( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQML_API virtual void destroyLayout() override;

private:

    ::fwGuiQml::container::QmlContainer::sptr m_parentContainer;

    /// Layout manager of this view
    QPointer<QMainWindow> m_qmlWindow;
};

} // namespace fwGui
