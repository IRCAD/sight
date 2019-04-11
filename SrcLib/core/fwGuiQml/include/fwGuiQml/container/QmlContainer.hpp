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

//Item is a children of container
#pragma once

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

class QLayout;

namespace fwGuiQml
{
namespace container
{

/**
 * @brief   Defines the QWidget container for UI.
 *
 */

class FWGUIQML_CLASS_API QmlContainer : public ::fwGui::container::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QmlContainer)(::fwGui::container::fwContainer),
                                            (()),
                                            ::fwGui::factory::New< QmlContainer > )

    FWGUIQML_API QmlContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQML_API virtual ~QmlContainer() noexcept;

//    FWGUIQML_API virtual void setQmlContainer(QWidget* container);
//    FWGUIQML_API virtual QWidget* getQmlContainer();

    /**
     * @brief Assign a layout to the container widget. Previous layout and its children are deleted and the container
     *  widget take the ownership of the new layout
     *
     * @param The layout to set on the container widget
     */
    FWGUIQML_API virtual void setLayout(QLayout* const layout);

    FWGUIQML_API virtual void clean() override;
    FWGUIQML_API virtual void destroyContainer() override;
    FWGUIQML_API virtual bool isShownOnScreen() override;

    FWGUIQML_API virtual void setVisible(bool isVisible) override;
    FWGUIQML_API virtual void setEnabled(bool isEnabled) override;

private:

//    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace fwGuiQt
