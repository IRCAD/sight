/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
#include "preferences_configuration.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/Preferences.hpp>

#include <boost/algorithm/string.hpp>
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

const core::com::signals::key_t preferences_configuration::PARAMETERS_MODIFIED_SIG = "parametersModified";
const core::com::signals::key_t preferences_configuration::PREFERENCE_CHANGED_SIG  = "preferenceChanged";

//------------------------------------------------------------------------------

sight::ui::parameter_t preferences_configuration::convertValue(const PreferenceElt& _elt)
{
    switch(_elt.m_type)
    {
        case PreferenceType::TEXT:
        case PreferenceType::PATH:
        case PreferenceType::COMBOBOX:
        case PreferenceType::FILE:
        {
            return _elt.m_preferenceValue;
        }

        case PreferenceType::U_INT:
        {
            return std::stoi(_elt.m_preferenceValue);
        }

        case PreferenceType::DOUBLE:
        {
            return std::stod(_elt.m_preferenceValue);
        }

        case PreferenceType::CHECKBOX:
        {
            return _elt.m_preferenceValue == "true";
        }

        case PreferenceType::LIST:
        {
            std::vector<std::string> list;
            boost::split(list, _elt.m_preferenceValue, boost::is_any_of(_elt.m_separator));
            return list;
        }

        default:
        {
            SIGHT_ASSERT("Preference type not handled", false);
            return {};
        }
    }
}

//-----------------------------------------------------------------------------

preferences_configuration::preferences_configuration() noexcept
{
    m_sigParametersModified = new_signal<ParametersModifiedSignalType>(PARAMETERS_MODIFIED_SIG);
    m_sigPreferenceChanged  = new_signal<ChangedSignalType>(PREFERENCE_CHANGED_SIG);
}

//------------------------------------------------------------------------------

preferences_configuration::~preferences_configuration() noexcept =
    default;

//------------------------------------------------------------------------------

void preferences_configuration::configuring()
{
    this->initialize();

    const auto configuration = this->get_config();

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
        else if(type == "list")
        {
            pref.m_type = PreferenceType::LIST;

            pref.m_separator = cfg.second.get("separator", ",");
        }
        else
        {
            SIGHT_ERROR("Preference type " << type << " is not implemented");
        }

        pref.m_name          = cfg.second.get<std::string>("name");
        pref.m_preferenceKey = cfg.second.get<std::string>("key");
        pref.m_defaultValue  = cfg.second.get<std::string>("default_value");

        if(pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
           || pref.m_type == PreferenceType::FILE || pref.m_type == PreferenceType::LIST)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::CHECKBOX)
        {
            pref.m_checkBox = new QCheckBox();
            pref.m_checkBox->setChecked(pref.m_defaultValue == "true");
            pref.m_checkBox->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::U_INT)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QIntValidator(pref.m_iMinMax.first, pref.m_iMinMax.second));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::DOUBLE)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QDoubleValidator(pref.m_dMinMax.first, pref.m_dMinMax.second, 6));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == PreferenceType::COMBOBOX)
        {
            const auto valuesCfg = cfg.second.get<std::string>("values");

            const boost::char_separator<char> sep(", ;");
            const boost::tokenizer<boost::char_separator<char> > tokens {valuesCfg, sep};

            pref.m_comboBox = new QComboBox();
            pref.m_comboBox->setObjectName(pref.m_preferenceKey.c_str());
            for(const std::string& value : tokens)
            {
                pref.m_comboBox->addItem(QString::fromStdString(value));
            }
        }

        m_preferences.push_back(pref);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::starting()
{
    this->actionServiceStarting();

    try
    {
        sight::ui::Preferences preferences;

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
    catch(const sight::ui::PreferencesDisabled& /*e*/)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::updating()
{
    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QPointer<QDialog> dialog = new QDialog();
    dialog->setObjectName("preferences_configuration");
    QPointer<QGridLayout> layout = new QGridLayout();

    int index = 0;
    for(PreferenceElt& pref : m_preferences)
    {
        QPointer<QLabel> label = new QLabel(QString::fromStdString(pref.m_name));
        layout->addWidget(label, index, 0);

        if(pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::LIST)
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
                    sight::module::ui::qt::preferences_configuration::onSelectDir(pref.m_lineEdit);
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
                    sight::module::ui::qt::preferences_configuration::onSelectFile(pref.m_lineEdit);
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
    cancelButton->setObjectName(cancelButton->text());
    QPointer<QPushButton> okButton = new QPushButton("OK");
    okButton->setObjectName(okButton->text());
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
        sight::ui::Preferences preferences;

        for(PreferenceElt& pref : m_preferences)
        {
            bool preferenceUpdate = false; // only emit signal for preference that has changed.

            if((pref.m_type == PreferenceType::TEXT || pref.m_type == PreferenceType::PATH
                || pref.m_type == PreferenceType::FILE || pref.m_type == PreferenceType::LIST)
               && !pref.m_lineEdit->text().isEmpty())
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
                const auto value = this->convertValue(pref);
                m_sigPreferenceChanged->async_emit(value, pref.m_preferenceKey);
            }
        }

        m_sigParametersModified->async_emit();
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void preferences_configuration::onSelectDir(QPointer<QLineEdit> lineEdit)
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle("Select Storage directory");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(sight::ui::dialog::location::WRITE);
    dialogFile.setType(sight::ui::dialog::location::FOLDER);

    const auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_folder());
        lineEdit->setText(QString::fromStdString(result->get_folder().string()));
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::onSelectFile(QPointer<QLineEdit> lineEdit)
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle("Select File");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(sight::ui::dialog::location::READ);
    dialogFile.setType(sight::ui::dialog::location::SINGLE_FILE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_file().parent_path());
        lineEdit->setText(QString::fromStdString(result->get_file().string()));
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
