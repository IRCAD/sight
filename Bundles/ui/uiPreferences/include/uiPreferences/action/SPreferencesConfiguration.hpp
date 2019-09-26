/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiPreferences/config.hpp"

#include <fwCom/Signals.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwTools/Failed.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>

#include <vector>

namespace fwData
{
class String;
}

namespace uiPreferences
{

namespace action
{

/**
 * @brief   This action shows a dialog to configure preferences of an application.
 *
 * @note    This service works with preferences and does nothing if the preferences service is not found.
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
       </service>
   @endcode
 * - \b type: the type of the parameter field (path, text, checkbox, int, double, combobox).
 * - \b name: the name of the parameter.
 * - \b key: the key of the parameter.
 * - \b default_value: the default value of the parameter.
 * - \b values: list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for 'combobox'
 * type).
 */
class UIPREFERENCES_CLASS_API SPreferencesConfiguration : public QObject,
                                                          public ::fwGui::IActionSrv
{
Q_OBJECT
public:

    fwCoreServiceMacro(SPreferencesConfiguration, ::fwGui::IActionSrv);

    /// Constructor. Do nothing.
    UIPREFERENCES_API SPreferencesConfiguration() noexcept;

    /// Destructor. Do nothing.
    UIPREFERENCES_API virtual ~SPreferencesConfiguration() noexcept;

    /// Type of signal when parameters are updated.
    typedef ::fwCom::Signal< void () > ParametersModifiedSignalType;
    UIPREFERENCES_API static const ::fwCom::Signals::SignalKeyType s_PARAMETERS_MODIFIED_SIG;

protected:

    /// Start the action. Gets the preference composite
    virtual void starting() override;

    /// Does nothing
    virtual void stopping() override;

    /// Shows a dialog to configure preferences declared in xml.
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

    /// Configures the service
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

    void onSelectDir(QPointer<QLineEdit> lineEdit);

private:

    enum class PreferenceType : std::int8_t
    {
        TEXT,
        CHECKBOX,
        U_INT,
        PATH,
        COMBOBOX,
        DOUBLE
    };

    struct PreferenceElt
    {
        PreferenceType m_type;
        QPointer<QLineEdit> m_lineEdit;
        QPointer<QCheckBox> m_checkBox;
        QPointer<QComboBox> m_comboBox;
        SPTR(::fwData::String) m_dataPreference;
        std::string m_preferenceKey;
        std::string m_name;
        std::string m_defaultValue;
    };

    ParametersModifiedSignalType::sptr m_sigParametersModified;

    typedef std::vector<PreferenceElt> PreferenceEltType;
    PreferenceEltType m_preferences;
};

} // namespace action
} // namespace uiPreferences
