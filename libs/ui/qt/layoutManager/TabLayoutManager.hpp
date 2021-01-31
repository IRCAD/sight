/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/config.hpp"
#include "ui/qt/container/QtContainer.hpp"

#include <core/base.hpp>

#include <QPointer>

#include <ui/base/layoutManager/TabLayoutManagerBase.hpp>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace sight::ui::qt
{

/**
 * @brief   Defines the tab layout manager.
 */
class UI_QT_CLASS_API TabLayoutManager : public ui::base::layoutManager::TabLayoutManagerBase
{

public:

    fwCoreClassMacro(TabLayoutManager, ui::base::layoutManager::TabLayoutManagerBase,
                     ui::base::factory::New< TabLayoutManager >)

    UI_QT_API TabLayoutManager(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~TabLayoutManager();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    UI_QT_API virtual void createLayout( ui::base::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_QT_API virtual void destroyLayout() override;

private:

    ui::qt::container::QtContainer::sptr m_parentContainer;

    // Layout manager of this view
    QPointer<QTabWidget> m_tabWidget;
};

} // namespace sight::ui::qt
