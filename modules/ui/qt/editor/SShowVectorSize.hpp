/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <core/tools/Failed.hpp>

#include <data/Vector.hpp>

#include <QLabel>
#include <QPointer>

#include <ui/base/editor/IEditor.hpp>

class QPushButton;

namespace sight::modules::ui::qt
{

namespace editor
{

/**
 * @brief  SShowVectorSize service show the size of a data::Vector
 *
 * @code{.xml}
    <service uid="..." type="::sight::modules::ui::qt::editor::SShowVectorSize" autoConnect="yes">
        <in key="vector" uid="..." autoConnect="yes"/>
        <text> my text </text>
    </service>
   @endcode
 * - \b text (optional, by default "") : the text to show before size of the vector
 *
 * @subsection Inputs Inputs
 * - \b vector [sight::data::Vector]: input vector.
 *
 * @subsection Slots Slots
 * -\b addObjects(data::Vector::ContainerType): increment displayed size of the vector.
 * -\b removeObjects(data::Vector::ContainerType ): decrement displayed size of the vector.
 *
 */
class MODULE_UI_QT_CLASS_API SShowVectorSize : public QObject,
                                               public sight::ui::base::editor::IEditor
{

public:

    fwCoreServiceMacro(SShowVectorSize, sight::ui::base::editor::IEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SShowVectorSize() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SShowVectorSize() noexcept;

    /**
     * @name Slots API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_ADD_OBJECTS_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT;

    /** @} */

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Overrides
    virtual void updating() override;

    /// Overrides
    virtual void swapping() override;

    /// connect data::Vector's signals to SShowVector
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Configures the service
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

private:

    /// SLOT: add objects
    void addObjects(data::Vector::ContainerType objects);

    /// SLOT: remove objects
    void removeObjects(data::Vector::ContainerType objects);

    size_t m_vectorSize; ///< size of the vector
    QPointer< QLabel > m_label; ///< label where the text will be displayed
    QString m_textToShow; ///< text to show next to the size
};

} // namespace editor
} // namespace sight::modules::ui::qt
