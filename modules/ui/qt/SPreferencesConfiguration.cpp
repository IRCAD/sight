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
const core::com::Signals::SignalKeyType SPreferencesConfiguration::s_PREFERENCE_CHANGED_SIG  = "preferenceChanged";

//------------------------------------------------------------------------------

sight::ui::base::parameter_t SPreferencesConfiguration::convertValue(
    const PreferenceType& _type,
    const std::string& _stringValue
)
{
    switch(_type)
    {
        case PreferenceType::TEXT:
        case PreferenceType::PATH:
        case PreferenceType::COMBOBOX:
        case PreferenceType::FILE:
        {
            return _stringValue;
        }

        case PreferenceType::U_INT:
        {
            return std::stoi(_stringValue);
        }

        case PreferenceType::DOUBLE:
        {
            return std::stod(_stringValue);
        }

        case PreferenceType::CHECKBOX:
        {
            return _stringValue == "true";
        }

        default:
        {
            SIGHT_ASSERT("Preference type not handled", false);
            return {};
        }
    }
}

//-----------------------------------------------------------------------------

SPreferencesConfiguration::SPreferencesConfiguration() noexcept
{
    m_sigParametersModified = newSignal<ParametersModifiedSignalType>(s_PARAMETERS_MODIFIED_SIG);
    m_sigPreferenceChanged  = newSignal<ChangedSignalType>(s_PREFERENCE_CHANGED_SIG);
}

//------------------------------------------------------------------------------

SPreferencesConfiguration::~SPreferencesConfiguration() noexcept =
    default;

//------------------------------------------------------------------------------

void SPreferencesConfiguration::configuring()
{
    this->initialize();

    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    const auto configuration = this->getConfigTree();

    for(const auto& cfg : boost::make_iterator_range(configuration.equal_range("preference")))
    {
        PreferenceElt pref;

        auto type = cfg.second.get<std::string>("type");
        if(type == "checkbox")
        {
            pref.m_type = PreferenceType::CHECKBOX;
        }
        else if(type == "text")
        {
            pref.m_type = PreferenceType::TEXT;
        }
        else if(type == "path")
        {
            pref.m_type = PreferenceType::PATH;
        }
        else if(type == "file")
        {
            pref.m_type = PreferenceType::FILE;
        }
        else if(type == "combobox")
        {
            pref.m_type = PreferenceType::COMBOBOX;
        }
        else if(type == "double")
        {
            pref.m_type = PreferenceType::DOUBLE;

            pref.m_dMinMax.first  = cfg.second.get<double>("min", pref.m_dMinMax.first);
            pref.m_dMinMax.second = cfg.second.get<double>("max", pref.m_dMinMax.second);
        }
        else if(type == "int")
        {
            pref.m_type = PreferenceType::U_INT;

            pref.m_iMinMax.first  = cfg.second.get<int>("min", pref.m_iMinMax.first);
            pref.m_iMinMax.second = cfg.second.get<int>("max", pref.m_iMinMax.second);
        }
        else
        {
            SIGHT_ERROR("Preference type " << type << " is not implemented");
        }

        pref.m_name          = cfg.second.get<std::string>("name");
        pref.m_preferenceKey = cfg.second.get<std::string>("key");
        pref.m_defaultValue  = cfg.second.get<std::string>("default_value");

        if(pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
           || pref.m_type == PreferenceType::FILE)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setObjectName(serviceID + '/' + pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::CHECKBOX)
        {
            pref.m_checkBox = new QCheckBox();
            pref.m_checkBox->setChecked(pref.m_defaultValue == "true");
            pref.m_checkBox->setObjectName(serviceID + '/' + pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::U_INT)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QIntValidator(pref.m_iMinMax.first, pref.m_iMinMax.second));
            pref.m_lineEdit->setObjectName(serviceID + '/' + pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::DOUBLE)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QDoubleValidator(pref.m_dMinMax.first, pref.m_dMinMax.second, 6));
            pref.m_lineEdit->setObjectName(serviceID + '/' + pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            const auto valuesCfg = cfg.second.get<std::string>("values");

            const boost::char_separator<char> sep(", ;");
            const boost::tokenizer<boost::char_separator<char> > tokens {valuesCfg, sep};

            pref.m_comboBox = new QComboBox();
            pref.m_comboBox->setObjectName(serviceID + '/' + pref.m_preferenceKey.c_str());
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
    catch(const sight::ui::base::PreferencesDisabled& /*e*/)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

void SPreferencesConfiguration::updating()
{
    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    QPointer<QDialog> dialog = new QDialog();
    dialog->setObjectName(serviceID);
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
                [pref]()
                {
                    sight::module::ui::qt::SPreferencesConfiguration::onSelectDir(pref.m_lineEdit);
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
                [pref]()
                {
                    sight::module::ui::qt::SPreferencesConfiguration::onSelectFile(pref.m_lineEdit);
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
    cancelButton->setObjectName(serviceID + '/' + cancelButton->text());
    QPointer<QPushButton> okButton = new QPushButton("OK");
    okButton->setObjectName(serviceID + '/' + okButton->text());
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
            bool preferenceUpdate = false; // only emit signal for preference that has changed.

            if((pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
                || pref.m_type == PreferenceType::FILE) && !pref.m_lineEdit->text().isEmpty())
            {
                preferenceUpdate =
                    pref.m_preferenceValue != pref.m_lineEdit->text().toStdString();
                pref.m_preferenceValue = pref.m_lineEdit->text().toStdString();
            }
            else if(pref.m_type == PreferenceType::CHECKBOX)
            {
                const std::string checked = pref.m_checkBox->isChecked() ? "true" : "false";
                preferenceUpdate       = pref.m_preferenceValue != checked;
                pref.m_preferenceValue = checked;
            }
            else if(pref.m_type == PreferenceType::U_INT || pref.m_type == PreferenceType::DOUBLE)
            {
                int pos               = 0;
                QLineEdit* const edit = pref.m_lineEdit;
                QString text          = edit->text();
                const bool isValid    = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                if(isValid)
                {
                    preferenceUpdate =
                        pref.m_preferenceValue != pref.m_lineEdit->text().toStdString();
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
                preferenceUpdate =
                    pref.m_preferenceValue != pref.m_comboBox->currentText().toStdString();
                pref.m_preferenceValue = pref.m_comboBox->currentText().toStdString();
            }

            preferences.put(pref.m_preferenceKey, pref.m_preferenceValue);

            // Emit preferenceChanged signal with new value and preference key.
            if(preferenceUpdate)
            {
                const auto value = this->convertValue(pref.m_type, pref.m_preferenceValue);
                m_sigPreferenceChanged->asyncEmit(value, pref.m_preferenceKey);
            }
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
