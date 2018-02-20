/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "uiPreferences/action/SPreferencesConfiguration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>

namespace uiPreferences
{

namespace action
{

const ::fwCom::Signals::SignalKeyType SPreferencesConfiguration::s_PARAMETERS_MODIFIED_SIG = "parametersModified";

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiPreferences::action::SPreferencesConfiguration, ::fwData::Object );

//-----------------------------------------------------------------------------

SPreferencesConfiguration::SPreferencesConfiguration() noexcept
{
    m_sigParametersModified = newSignal< ParametersModifiedSignalType >(s_PARAMETERS_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

SPreferencesConfiguration::~SPreferencesConfiguration() noexcept
{
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::starting()
{
    this->actionServiceStarting();

    // Check preferences
    auto preferencesServicesList = ::fwServices::OSR::getServices("::fwPreferences::IPreferences");
    if(!preferencesServicesList.empty())
    {
        const ::fwServices::IService::sptr prefService = *preferencesServicesList.begin();
        ::fwData::Composite::sptr prefs = prefService->getObject< ::fwData::Composite >();

        for(PreferenceElt& pref : m_preferences)
        {
            pref.m_dataPreference                      = ::fwData::String::New(pref.m_defaultValue);
            ::fwData::Composite::IteratorType iterPref = prefs->find( pref.m_preferenceKey );
            if ( iterPref != prefs->end() )
            {
                pref.m_dataPreference = ::fwData::String::dynamicCast(iterPref->second);
            }
            else
            {
                (*prefs)[pref.m_preferenceKey] = pref.m_dataPreference;
            }
        }
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::configuring()
{
    this->initialize();

    ::fwRuntime::ConfigurationElementContainer config = m_configuration->findAllConfigurationElement("preference");
    for(const auto& elt : config.getElements())
    {
        PreferenceElt pref;

        ConfigurationType typeCfg = elt->findConfigurationElement("type");
        SLM_ASSERT("element 'type' is missing.", typeCfg);
        if(typeCfg->getValue() == "checkbox")
        {
            pref.m_type = PreferenceType::CHECKBOX;
        }
        else if (typeCfg->getValue() == "text" )
        {
            pref.m_type = PreferenceType::TEXT;
        }
        else if (typeCfg->getValue() == "number")
        {
            pref.m_type = PreferenceType::U_INT;
        }
        else if(typeCfg->getValue() == "path")
        {
            pref.m_type = PreferenceType::PATH;
        }
        else if(typeCfg->getValue() == "combobox")
        {
            pref.m_type = PreferenceType::COMBOBOX;
        }
        else
        {
            OSLM_ERROR("Preference type "<<typeCfg->getValue()<<" is not implemented");
        }

        ConfigurationType nameCfg = elt->findConfigurationElement("name");
        SLM_ASSERT("element 'name' is missing.", nameCfg);
        pref.m_name = nameCfg->getValue();

        ConfigurationType keyCfg = elt->findConfigurationElement("key");
        SLM_ASSERT("element 'key' is missing.", keyCfg);
        pref.m_preferenceKey = keyCfg->getValue();

        ConfigurationType defaultValueCfg = elt->findConfigurationElement("default_value");
        SLM_ASSERT("element 'default_value' is missing.", defaultValueCfg);
        pref.m_defaultValue = defaultValueCfg->getValue();

        if(pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
        }
        else if(pref.m_type == PreferenceType::CHECKBOX)
        {
            pref.m_checkBox = new QCheckBox();
            pref.m_checkBox->setChecked(pref.m_defaultValue == "true");
        }
        else if(pref.m_type == PreferenceType::U_INT)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator( new QIntValidator( 0, 999999));
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            const ::boost::char_separator<char> sep(", ;");
            const ::boost::tokenizer< ::boost::char_separator<char> > tokens {pref.m_defaultValue, sep};

            pref.m_comboBox = new QComboBox();
            for(const std::string& value : tokens)
            {
                pref.m_comboBox->addItem(QString::fromStdString(value));
            }
        }
        m_preferences.push_back(pref);
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::updating()
{
    QPointer<QDialog> dialog     = new QDialog();
    QPointer<QGridLayout> layout = new QGridLayout();

    int index = 0;
    for(PreferenceElt& pref : m_preferences)
    {
        QPointer<QLabel> label = new QLabel(QString::fromStdString(pref.m_name));
        layout->addWidget(label, index, 0);

        if(pref.m_type == PreferenceType::TEXT)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_dataPreference->value()));
            layout->addWidget(pref.m_lineEdit, index, 1);
        }
        else if(pref.m_type == PreferenceType::CHECKBOX)
        {
            pref.m_checkBox->setChecked(pref.m_dataPreference->value() == "true");
            layout->addWidget(pref.m_checkBox, index, 1);
        }
        else if(pref.m_type == PreferenceType::U_INT)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_dataPreference->value()));
            layout->addWidget(pref.m_lineEdit, index, 1);
        }
        else if(pref.m_type == PreferenceType::PATH)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_dataPreference->value()));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QPointer<QPushButton> directorySelector = new QPushButton("...");
            layout->addWidget(directorySelector, index, 2);
            QObject::connect(directorySelector.data(), &QPushButton::clicked, [this, pref]()
                    {
                        this->onSelectDir(pref.m_lineEdit);
                    });
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            const int currentIndex = pref.m_comboBox->findText(QString::fromStdString(pref.m_dataPreference->value()));
            if(currentIndex < 0)
            {
                OSLM_WARN( "Preference '" << pref.m_dataPreference->value() <<
                           "' can't be find in combobox. The first one is selected.");
                pref.m_comboBox->setCurrentIndex(0);
            }
            else
            {
                pref.m_comboBox->setCurrentIndex(currentIndex);
            }
            layout->addWidget(pref.m_comboBox, index, 1);
        }

        ++index;
    }

    QPointer<QPushButton> cancelButton = new QPushButton("Cancel");
    QPointer<QPushButton> okButton     = new QPushButton("OK");
    okButton->setDefault(true);

    QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    layout->addLayout(buttonLayout, index, 1, 4, 2 );

    QObject::connect(cancelButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::reject);
    QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

    dialog->setLayout(layout);

    if (dialog->exec() == QDialog::Accepted)
    {
        for(PreferenceElt& pref : m_preferences)
        {
            if((pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH) &&
               !pref.m_lineEdit->text().isEmpty())
            {
                pref.m_dataPreference->value() = pref.m_lineEdit->text().toStdString();
            }
            else if(pref.m_type == PreferenceType::CHECKBOX)
            {
                pref.m_dataPreference->value() = pref.m_checkBox->isChecked() ? "true" : "false";
            }
            else if(pref.m_type == PreferenceType::U_INT)
            {
                pref.m_dataPreference->value() = pref.m_lineEdit->text().toStdString();
            }
            else if(pref.m_type == PreferenceType::COMBOBOX)
            {
                pref.m_dataPreference->value() = pref.m_comboBox->currentText().toStdString();
            }
        }
        m_sigParametersModified->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::onSelectDir(QPointer<QLineEdit> lineEdit)
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select Storage directory");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        lineEdit->setText( QString::fromStdString(result->getFolder().string()) );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::swapping()
{
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace videoQt
