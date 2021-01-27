/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "uiPreferences/config.hpp"

#include <core/com/Signals.hpp>
#include <core/tools/Failed.hpp>

#include <data/String.hpp>

#include <gui/IActionSrv.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>

#include <vector>

namespace sight::data
{
class String;
}

namespace uiPreferences
{

namespace action
{

/**
 * @brief This action shows a dialog to configure preferences of an application.
 *
 * @note This service works with preferences and does nothing if the preferences service is not found.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::uiPreferences::action::SPreferencesConfiguration">
        <preference>
            <type>path</type>
            <name>Video path</name>
            <key>VIDEO_REF_DIR</key>
            <default_value>...</default_value>
        </preference>
        <preference>
            <type>file</type>
            <name>Video file</name>
            <key>VIDEO_REF_FILE</key>
            <default_value>...</default_value>
        </preference>
        <preference>
            <type>text</type>
            <name>DICOM/MPPS port</name>
            <key>DICOM_PORT_KEY</key>
            <default_value>11112</default_value>
        </preference>
        <preference>
            <type>text</type>
            <name>AE Title</name>
            <key>DICOM_AETITLE_KEY</key>
            <default_value>dicom</default_value>
        </preference>
        <preference>
            <type>checkbox</type>
            <name>Keep PixelData (C-STORE)</name>
            <key>KEEP_PIXELDATA_KEY</key>
            <default_value>true</default_value>
        </preference>
        <preference>
            <type>combobox</type>
            <name>Device name</name>
            <key>DEVICE_NAME</key>
            <values>trakStar,Aurora</values>
            <default_value>trakStar</default_value>
        </preference>
        <preference>
            <min>50</min>
            <max>500</max>
            <type>int</type>
            <name>Device port</name>
            <key>DEVICE_PORT</key>
            <default_value>104</default_value>
        </preference>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b type (mandatory, path/text/checkbox/int/double/combobox): the type of the parameter field.
 * - \b name (mandatory, string): the name of the parameter.
 * - \b key (mandatory, string): the key of the parameter.
 * - \b default_value (mandatory): the default value of the parameter.
 * - \b values (optional): list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for
 *                         'combobox' type).
 * - \b min (optional, int/double, default=0/-1000000.0): minimum value allowed in the field.
 * - \b max (optional, int/double, default=999999/1000000.0): maximum value allowed in the field.
 */
class UIPREFERENCES_CLASS_API SPreferencesConfiguration final :
    public QObject,
    public ::sight::gui::IActionSrv
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SPreferencesConfiguration, ::sight::gui::IActionSrv)

    /// Initializes the signal.
    UIPREFERENCES_API SPreferencesConfiguration() noexcept;

    /// Destroys the service.
    UIPREFERENCES_API ~SPreferencesConfiguration() noexcept override;

protected:

    /// Configures the service.
    UIPREFERENCES_API void configuring() override;

    /// Gets the preference composite.
    UIPREFERENCES_API void starting() override;

    /// Shows a dialog to configure preferences declared in xml.
    UIPREFERENCES_API void updating() override;

    /// Does nothing.
    UIPREFERENCES_API void stopping() override;

private:

    /// Type of signal when parameters are updated.
    typedef core::com::Signal< void () > ParametersModifiedSignalType;
    UIPREFERENCES_API static const core::com::Signals::SignalKeyType s_PARAMETERS_MODIFIED_SIG;

    enum class PreferenceType : std::int8_t
    {
        TEXT,
        CHECKBOX,
        U_INT,
        PATH,
        FILE,
        COMBOBOX,
        DOUBLE
    };

    struct PreferenceElt
    {
        PreferenceType m_type;
        QPointer<QLineEdit> m_lineEdit;
        QPointer<QCheckBox> m_checkBox;
        QPointer<QComboBox> m_comboBox;
        data::String::sptr m_dataPreference;
        std::string m_preferenceKey;
        std::string m_name;
        std::string m_defaultValue;
        std::pair< int, int > m_iMinMax { 0, 999999 };
        std::pair< double, double > m_dMinMax { -1000000.0, 1000000.0 };
    };

    void onSelectDir(QPointer<QLineEdit> _lineEdit);

    void onSelectFile(QPointer<QLineEdit> _lineEdit);

    ParametersModifiedSignalType::sptr m_sigParametersModified;

    std::vector<PreferenceElt> m_preferences;

};

} // namespace action.

} // namespace uiPreferences.
