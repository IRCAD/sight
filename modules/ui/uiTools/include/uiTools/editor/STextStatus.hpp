/************************************************************************
 *
 * Copyright (C) 2017-2020 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <fwGui/editor/IEditor.hpp>

#include <QPointer>

class QLabel;

namespace uiTools
{

namespace editor
{

/**
 * @brief This service is used to displays and update values (int, double or string) in a QLabel.
 *
 * @section Slots Slots
 * - \b setIntParameter(int): display the value in the QLabel.
 * - \b setDoubleParameter(double): display the value in the QLabel.
 * - \b setBoolParameter(int): display the value in the QLabel.
 * - \b setStringParameter(int): display the value in the QLabel.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::uiTools::editor::STextStatus">
        <in key="string" uid="..." />
        <label>my label</label>
        <color>#FF0000</color>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b string(::fwData::String, optional): string data to display.
 *
 * @subsection Configuration Configuration
 * - \b label (optional, default="") : text to show before size of the vector
 * - \b color (optional, default="red") : needed color of the displayed label in a CSS style as names (ex: red),
 * rgb/rgba (ex: rgb(0,255,137,0.3)) or hexadecimal (ex: #355C66).
 */
class UITOOLS_CLASS_API STextStatus final : public QObject,
                                            public ::fwGui::editor::IEditor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(STextStatus, ::fwGui::editor::IEditor)

    /// Initializes slots and member.
    UITOOLS_API STextStatus();

    /// Destroys the service.
    UITOOLS_API ~STextStatus() override;

private:

    /// Configures the service.
    void configuring() override;

    /// Installs the layout and gets the input data if it exists and displays it.
    void starting() override;

    /// Gets the input data if it exists and displays it.
    void updating() override;

    /// Destroys the layout.
    void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_STRING_INPUT to s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Sets the interger to display.
    void setIntParameter(int _val);

    /// Sets the double to display.
    void setDoubleParameter(double _val);

    /// Sets the boolean to display.
    void setBoolParameter(bool _val);

    /// Sets the string to display.
    void setStringParameter(std::string _val);

    /// Stores the label.
    QPointer< QLabel > m_labelValue;

    /// Stores the static text to be displayed.
    QPointer< QLabel > m_labelStaticText;

};

} // namespace editor.
} // namespace uiTools.
