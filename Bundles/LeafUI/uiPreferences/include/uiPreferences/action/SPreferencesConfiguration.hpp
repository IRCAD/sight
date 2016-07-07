/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __UIPREFERENCES_ACTION_SPREFERENCESCONFIGURATION_HPP__
#define __UIPREFERENCES_ACTION_SPREFERENCESCONFIGURATION_HPP__

#include "uiPreferences/config.hpp"

#include <fwCom/Signals.hpp>

#include <fwTools/Failed.hpp>

#include <fwGui/IActionSrv.hpp>

#include <vector>

#include <QCheckBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>

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
       </service>
   @endcode
 * - \b type : the type of the parameter field (checkBox, lineEdit).
 * - \b name : the name of the parameter.
 * - \b key  : the key of the parameter.
 * - \b default_value : the default value of the parameter.
 */
class UIPREFERENCES_CLASS_API SPreferencesConfiguration : public QObject,
                                                          public ::fwGui::IActionSrv
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SPreferencesConfiguration)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    UIPREFERENCES_API SPreferencesConfiguration() throw();

    /// Destructor. Do nothing.
    UIPREFERENCES_API virtual ~SPreferencesConfiguration() throw();

    /// Type of signal when parameters are updated.
    typedef ::fwCom::Signal< void () > ParametersModifiedSignalType;
    UIPREFERENCES_API static const ::fwCom::Signals::SignalKeyType s_PARAMETERS_MODIFIED_SIG;

protected:

    /// Start the action. Gets the preference composite
    virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    virtual void stopping() throw(::fwTools::Failed);

    /// Shows a dialog to configure preferences declared in xml.
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configures the service
    virtual void configuring() throw(::fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

    void onSelectDir(QPointer<QLineEdit> lineEdit);

private:

    enum class PreferenceType : std::int8_t
    {
        TEXT,
        CHECKBOX,
        U_INT,
        PATH
    };

    struct PreferenceElt
    {
        PreferenceType m_type;
        QPointer<QLineEdit> m_lineEdit;
        QPointer<QCheckBox> m_checkBox;
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

#endif /*__UIPREFERENCES_ACTION_SPREFERENCESCONFIGURATION_HPP__*/
