/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "uiTools/config.hpp"

#include <fwData/Vector.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QLabel>
#include <QPointer>

class QPushButton;

namespace uiTools
{

namespace editor
{

/**
 * @brief  SShowVectorSize service show the size of a fwData::Vector
 *
 * @code{.xml}
    <service uid="..." type="::uiTools::editor::SShowVectorSize" autoConnect="yes">
        <in key="vector" uid="..." autoConnect="yes"/>
        <text> my text </text>
    </service>
   @endcode
 * - \b text (optional, by default "") : the text to show before size of the vector
 *
 * @subsection Inputs Inputs
 * - \b vector [::fwData::Vector]: input vector.
 *
 * @subsection Slots Slots
 * -\b addObjects(::fwData::Vector::ContainerType): increment displayed size of the vector.
 * -\b removeObjects(::fwData::Vector::ContainerType ): decrement displayed size of the vector.
 *
 */
class UITOOLS_CLASS_API SShowVectorSize : public QObject,
                                          public ::fwGui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SShowVectorSize)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UITOOLS_API SShowVectorSize() noexcept;

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SShowVectorSize() noexcept;

    /**
     * @name Slots API
     * @{
     */
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT;

    /** @} */

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

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

    /// connect fwData::Vector's signals to SShowVector
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Configures the service
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

private:

    /// SLOT: add objects
    void addObjects(::fwData::Vector::ContainerType objects);

    /// SLOT: remove objects
    void removeObjects(::fwData::Vector::ContainerType objects);

    size_t m_vectorSize; ///< size of the vector
    QPointer< QLabel > m_label; ///< label where the text will be displayed
    QString m_textToShow; ///< text to show next to the size
};

} // namespace editor
} // namespace uiTools
