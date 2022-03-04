/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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
#include "SPreferencesConfiguration.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Composite.hpp>
#include <data/Integer.hpp>

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <boost/tokenizer.hpp>

#include <QApplication>
#include <QDialog>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>

namespace sight::module::ui::qt
{

const core::com::Signals::SignalKeyType SPreferencesConfiguration::s_PARAMETERS_MODIFIED_SIG = "parametersModified";

//-----------------------------------------------------------------------------

SPreferencesConfiguration::SPreferencesConfiguration() noexcept
{
    m_sigParametersModified = newSignal<ParametersModifiedSignalType>(s_PARAMETERS_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

SPreferencesConfiguration::~SPreferencesConfiguration() noexcept
{
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::configuring()
{
    this->initialize();

    core::runtime::ConfigurationElementContainer config = m_configuration->findAllConfigurationElement("preference");
    for(const auto& elt : config.getElements())
    {
        PreferenceElt pref;

        ConfigurationType typeCfg = elt->findConfigurationElement("type");
        SIGHT_ASSERT("element 'type' is missing.", typeCfg);
        if(typeCfg->getValue() == "checkbox")
        {
            pref.m_type = PreferenceType::CHECKBOX;
        }
        else if(typeCfg->getValue() == "text")
        {
            pref.m_type = PreferenceType::TEXT;
        }
        else if(typeCfg->getValue() == "path")
        {
            pref.m_type = PreferenceType::PATH;
        }
        else if(typeCfg->getValue() == "file")
        {
            pref.m_type = PreferenceType::FILE;
        }
        else if(typeCfg->getValue() == "combobox")
        {
            pref.m_type = PreferenceType::COMBOBOX;
        }
        else if(typeCfg->getValue() == "double")
        {
            pref.m_type = PreferenceType::DOUBLE;

            ConfigurationType keyCfg = elt->findConfigurationElement("min");
            if(keyCfg)
            {
                pref.m_dMinMax.first = std::stod(keyCfg->getValue());
            }

            keyCfg = elt->findConfigurationElement("max");
            if(keyCfg)
            {
                pref.m_dMinMax.second = std::stod(keyCfg->getValue());
            }
        }
        else if(typeCfg->getValue() == "int")
        {
            pref.m_type = PreferenceType::U_INT;

            ConfigurationType keyCfg = elt->findConfigurationElement("min");
            if(keyCfg)
            {
                pref.m_iMinMax.first = std::stoi(keyCfg->getValue());
            }

            keyCfg = elt->findConfigurationElement("max");
            if(keyCfg)
            {
                pref.m_iMinMax.second = std::stoi(keyCfg->getValue());
            }
        }
        else
        {
            SIGHT_ERROR("Preference type " << typeCfg->getValue() << " is not implemented");
        }

        ConfigurationType nameCfg = elt->findConfigurationElement("name");
        SIGHT_ASSERT("element 'name' is missing.", nameCfg);
        pref.m_name = nameCfg->getValue();

        ConfigurationType keyCfg = elt->findConfigurationElement("key");
        SIGHT_ASSERT("element 'key' is missing.", keyCfg);
        pref.m_preferenceKey = keyCfg->getValue();

        ConfigurationType defaultValueCfg = elt->findConfigurationElement("default_value");
        SIGHT_ASSERT("element 'default_value' is missing.", defaultValueCfg);
        pref.m_defaultValue = defaultValueCfg->getValue();

        if(pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
           || pref.m_type == PreferenceType::FILE)
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
            pref.m_lineEdit->setValidator(new QIntValidator(pref.m_iMinMax.first, pref.m_iMinMax.second));
        }
        else if(pref.m_type == PreferenceType::DOUBLE)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QDoubleValidator(pref.m_dMinMax.first, pref.m_dMinMax.second, 6));
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            ConfigurationType valuesCfg = elt->findConfigurationElement("values");
            SIGHT_ASSERT("element 'values' is missing.", valuesCfg);

            const boost::char_separator<char> sep(", ;");
            const std::string s = valuesCfg->getValue();
            const boost::tokenizer<boost::char_separator<char> > tokens {s, sep};

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

void SPreferencesConfiguration::starting()
{
    this->actionServiceStarting();

    try
    {
        sight::ui::base::Preferences preferences;

        for(auto& preference : m_preferences)
        {
            if(const auto& found = preferences.get_optional<std::string>(preference.m_preferenceKey); found)
            {
                preference.m_preferenceValue = *found;
            }
            else
            {
                preference.m_preferenceValue = preference.m_defaultValue;
                preferences.put(preference.m_preferenceKey, preference.m_defaultValue);
            }
        }
    }
    catch(const sight::ui::base::PreferencesDisabled& e)
    {
        // Nothing to do..
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
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
        }
        else if(pref.m_type == PreferenceType::CHECKBOX)
        {
            pref.m_checkBox->setChecked(pref.m_preferenceValue == "true");
            layout->addWidget(pref.m_checkBox, index, 1);
        }
        else if(pref.m_type == PreferenceType::U_INT || pref.m_type == PreferenceType::DOUBLE)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QObject::connect(
                pref.m_lineEdit,
                &QLineEdit::textEdited,
                [&]()
                {
                    int pos               = 0;
                    QLineEdit* const edit = pref.m_lineEdit;
                    QString text          = edit->text();
                    const bool isValid    = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                    if(qApp->styleSheet().isEmpty())
                    {
                        static const QColor defaultTextColor = QLineEdit().palette().color(QPalette::Text);

                        QPalette palette   = edit->palette();
                        const QColor color = isValid ? defaultTextColor : QColorConstants::Red;
                        palette.setColor(QPalette::Text, color);
                        edit->setPalette(palette);
                    }
                    else
                    {
                        edit->setProperty("type", isValid ? "" : "error");
                        edit->style()->unpolish(edit);
                        edit->style()->polish(edit);
                    }
                });
        }
        else if(pref.m_type == PreferenceType::PATH)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QPointer<QPushButton> directorySelector = new QPushButton("...");
            layout->addWidget(directorySelector, index, 2);
            QObject::connect(
                directorySelector.data(),
                &QPushButton::clicked,
                [this, pref]()
                {
                    this->onSelectDir(pref.m_lineEdit);
                });
        }
        else if(pref.m_type == PreferenceType::FILE)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QPointer<QPushButton> directorySelector = new QPushButton("...");
            layout->addWidget(directorySelector, index, 2);
            QObject::connect(
                directorySelector.data(),
                &QPushButton::clicked,
                [this, pref]()
                {
                    this->onSelectFile(pref.m_lineEdit);
                });
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            const int currentIndex = pref.m_comboBox->findText(QString::fromStdString(pref.m_preferenceValue));
            if(currentIndex < 0)
            {
                SIGHT_WARN(
                    "Preference '" + pref.m_preferenceValue
                    + "' can't be find in combobox. The first one is selected."
                );
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

    layout->addLayout(buttonLayout, index, 1, 4, 2);

    QObject::connect(cancelButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::reject);
    QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

    dialog->setLayout(layout);

    if(dialog->exec() == QDialog::Accepted)
    {
        sight::ui::base::Preferences preferences;

        for(PreferenceElt& pref : m_preferences)
        {
            if((pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
                || pref.m_type == PreferenceType::FILE) && !pref.m_lineEdit->text().isEmpty())
            {
                pref.m_preferenceValue = pref.m_lineEdit->text().toStdString();
            }
            else if(pref.m_type == PreferenceType::CHECKBOX)
            {
                pref.m_preferenceValue = pref.m_checkBox->isChecked() ? "true" : "false";
            }
            else if(pref.m_type == PreferenceType::U_INT || pref.m_type == PreferenceType::DOUBLE)
            {
                int pos               = 0;
                QLineEdit* const edit = pref.m_lineEdit;
                QString text          = edit->text();
                const bool isValid    = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                if(isValid)
                {
                    pref.m_preferenceValue = pref.m_lineEdit->text().toStdString();
                }

                if(qApp->styleSheet().isEmpty())
                {
                    static const QColor defaultTextColor = QLineEdit().palette().color(QPalette::Text);

                    QPalette palette = edit->palette();
                    palette.setColor(QPalette::Text, defaultTextColor);
                }
                else
                {
                    edit->setProperty("type", "");
                    edit->style()->unpolish(edit);
                    edit->style()->polish(edit);
                }
            }
            else if(pref.m_type == PreferenceType::COMBOBOX)
            {
                pref.m_preferenceValue = pref.m_comboBox->currentText().toStdString();
            }

            preferences.put(pref.m_preferenceKey, pref.m_preferenceValue);
        }

        m_sigParametersModified->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::onSelectDir(QPointer<QLineEdit> lineEdit)
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select Storage directory");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(sight::ui::base::dialog::ILocationDialog::FOLDER);

    const auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFolder());
        lineEdit->setText(QString::fromStdString(result->getFolder().string()));
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::onSelectFile(QPointer<QLineEdit> lineEdit)
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select File");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(sight::ui::base::dialog::ILocationDialog::SINGLE_FILE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFile().parent_path());
        lineEdit->setText(QString::fromStdString(result->getFile().string()));
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
