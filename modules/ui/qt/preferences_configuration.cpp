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
#include <ui/__/preferences.hpp>

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
        case preference_t::TEXT:
        case preference_t::PATH:
        case preference_t::COMBOBOX:
        case preference_t::FILE:
        {
            return _elt.m_preferenceValue;
        }

        case preference_t::U_INT:
        {
            return std::stoi(_elt.m_preferenceValue);
        }

        case preference_t::DOUBLE:
        {
            return std::stod(_elt.m_preferenceValue);
        }

        case preference_t::CHECKBOX:
        {
            return _elt.m_preferenceValue == "true";
        }

        case preference_t::LIST:
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
    m_sigParametersModified = new_signal<parameters_modified_signal_t>(PARAMETERS_MODIFIED_SIG);
    m_sigPreferenceChanged  = new_signal<changed_signal_t>(PREFERENCE_CHANGED_SIG);
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
            pref.m_type = preference_t::CHECKBOX;
        }
        else if(type == "text")
        {
            pref.m_type = preference_t::TEXT;
        }
        else if(type == "path")
        {
            pref.m_type = preference_t::PATH;
        }
        else if(type == "file")
        {
            pref.m_type = preference_t::FILE;
        }
        else if(type == "combobox")
        {
            pref.m_type = preference_t::COMBOBOX;
        }
        else if(type == "double")
        {
            pref.m_type = preference_t::DOUBLE;

            pref.m_dMinMax.first  = cfg.second.get<double>("min", pref.m_dMinMax.first);
            pref.m_dMinMax.second = cfg.second.get<double>("max", pref.m_dMinMax.second);
        }
        else if(type == "int")
        {
            pref.m_type = preference_t::U_INT;

            pref.m_iMinMax.first  = cfg.second.get<int>("min", pref.m_iMinMax.first);
            pref.m_iMinMax.second = cfg.second.get<int>("max", pref.m_iMinMax.second);
        }
        else if(type == "list")
        {
            pref.m_type = preference_t::LIST;

            pref.m_separator = cfg.second.get("separator", ",");
        }
        else
        {
            SIGHT_ERROR("Preference type " << type << " is not implemented");
        }

        pref.m_name          = cfg.second.get<std::string>("name");
        pref.m_preferenceKey = cfg.second.get<std::string>("key");
        pref.m_defaultValue  = cfg.second.get<std::string>("default_value");

        if(pref.m_type == preference_t::TEXT || pref.m_type == preference_t::PATH
           || pref.m_type == preference_t::FILE || pref.m_type == preference_t::LIST)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == preference_t::CHECKBOX)
        {
            pref.m_checkBox = new QCheckBox();
            pref.m_checkBox->setChecked(pref.m_defaultValue == "true");
            pref.m_checkBox->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == preference_t::U_INT)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QIntValidator(pref.m_iMinMax.first, pref.m_iMinMax.second));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == preference_t::DOUBLE)
        {
            pref.m_lineEdit = new QLineEdit(QString::fromStdString(pref.m_defaultValue));
            pref.m_lineEdit->setValidator(new QDoubleValidator(pref.m_dMinMax.first, pref.m_dMinMax.second, 6));
            pref.m_lineEdit->setObjectName(pref.m_preferenceKey.c_str());
        }
        else if(pref.m_type == preference_t::COMBOBOX)
        {
            const auto values_cfg = cfg.second.get<std::string>("values");

            const boost::char_separator<char> sep(", ;");
            const boost::tokenizer<boost::char_separator<char> > tokens {values_cfg, sep};

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
        sight::ui::preferences preferences;

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
    catch(const sight::ui::preferences_disabled& /*e*/)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::updating()
{
    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QPointer<QDialog> dialog = new QDialog();
    dialog->setObjectName("preferences_configuration");
    QPointer<QGridLayout> layout = new QGridLayout();

    int index = 0;
    for(PreferenceElt& pref : m_preferences)
    {
        QPointer<QLabel> label = new QLabel(QString::fromStdString(pref.m_name));
        layout->addWidget(label, index, 0);

        if(pref.m_type == preference_t::TEXT || pref.m_type == preference_t::LIST)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
        }
        else if(pref.m_type == preference_t::CHECKBOX)
        {
            pref.m_checkBox->setChecked(pref.m_preferenceValue == "true");
            layout->addWidget(pref.m_checkBox, index, 1);
        }
        else if(pref.m_type == preference_t::U_INT || pref.m_type == preference_t::DOUBLE)
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
                    const bool is_valid   = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                    if(qApp->styleSheet().isEmpty())
                    {
                        static const QColor defaultTextColor = QLineEdit().palette().color(QPalette::Text);

                        QPalette palette   = edit->palette();
                        const QColor color = is_valid ? defaultTextColor : QColorConstants::Red;
                        palette.setColor(QPalette::Text, color);
                        edit->setPalette(palette);
                    }
                    else
                    {
                        edit->setProperty("type", is_valid ? "" : "error");
                        edit->style()->unpolish(edit);
                        edit->style()->polish(edit);
                    }
                });
        }
        else if(pref.m_type == preference_t::PATH)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QPointer<QPushButton> directory_selector = new QPushButton("...");
            layout->addWidget(directory_selector, index, 2);
            QObject::connect(
                directory_selector.data(),
                &QPushButton::clicked,
                [pref]()
                {
                    sight::module::ui::qt::preferences_configuration::onSelectDir(pref.m_lineEdit);
                });
        }
        else if(pref.m_type == preference_t::FILE)
        {
            pref.m_lineEdit->setText(QString::fromStdString(pref.m_preferenceValue));
            layout->addWidget(pref.m_lineEdit, index, 1);
            QPointer<QPushButton> directory_selector = new QPushButton("...");
            layout->addWidget(directory_selector, index, 2);
            QObject::connect(
                directory_selector.data(),
                &QPushButton::clicked,
                [pref]()
                {
                    sight::module::ui::qt::preferences_configuration::onSelectFile(pref.m_lineEdit);
                });
        }
        else if(pref.m_type == preference_t::COMBOBOX)
        {
            const int current_index = pref.m_comboBox->findText(QString::fromStdString(pref.m_preferenceValue));
            if(current_index < 0)
            {
                SIGHT_WARN(
                    "Preference '" + pref.m_preferenceValue
                    + "' can't be find in combobox. The first one is selected."
                );
                pref.m_comboBox->setCurrentIndex(0);
            }
            else
            {
                pref.m_comboBox->setCurrentIndex(current_index);
            }

            layout->addWidget(pref.m_comboBox, index, 1);
        }

        ++index;
    }

    QPointer<QPushButton> cancel_button = new QPushButton("Cancel");
    cancel_button->setObjectName(cancel_button->text());
    QPointer<QPushButton> ok_button = new QPushButton("OK");
    ok_button->setObjectName(ok_button->text());
    ok_button->setDefault(true);

    QPointer<QHBoxLayout> button_layout = new QHBoxLayout();
    button_layout->addWidget(cancel_button);
    button_layout->addWidget(ok_button);

    layout->addLayout(button_layout, index, 1, 4, 2);

    QObject::connect(cancel_button.data(), &QPushButton::clicked, dialog.data(), &QDialog::reject);
    QObject::connect(ok_button.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

    dialog->setLayout(layout);

    if(dialog->exec() == QDialog::Accepted)
    {
        sight::ui::preferences preferences;

        for(PreferenceElt& pref : m_preferences)
        {
            bool preference_update = false; // only emit signal for preference that has changed.

            if((pref.m_type == preference_t::TEXT || pref.m_type == preference_t::PATH
                || pref.m_type == preference_t::FILE || pref.m_type == preference_t::LIST)
               && !pref.m_lineEdit->text().isEmpty())
            {
                preference_update =
                    pref.m_preferenceValue != pref.m_lineEdit->text().toStdString();
                pref.m_preferenceValue = pref.m_lineEdit->text().toStdString();
            }
            else if(pref.m_type == preference_t::CHECKBOX)
            {
                const std::string checked = pref.m_checkBox->isChecked() ? "true" : "false";
                preference_update      = pref.m_preferenceValue != checked;
                pref.m_preferenceValue = checked;
            }
            else if(pref.m_type == preference_t::U_INT || pref.m_type == preference_t::DOUBLE)
            {
                int pos               = 0;
                QLineEdit* const edit = pref.m_lineEdit;
                QString text          = edit->text();
                const bool is_valid   = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                if(is_valid)
                {
                    preference_update =
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
            else if(pref.m_type == preference_t::COMBOBOX)
            {
                preference_update =
                    pref.m_preferenceValue != pref.m_comboBox->currentText().toStdString();
                pref.m_preferenceValue = pref.m_comboBox->currentText().toStdString();
            }

            preferences.put(pref.m_preferenceKey, pref.m_preferenceValue);

            // Emit preferenceChanged signal with new value and preference key.
            if(preference_update)
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

void preferences_configuration::onSelectDir(QPointer<QLineEdit> _line_edit)
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle("Select Storage directory");
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(sight::ui::dialog::location::WRITE);
    dialog_file.setType(sight::ui::dialog::location::FOLDER);

    const auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_folder());
        _line_edit->setText(QString::fromStdString(result->get_folder().string()));
        dialog_file.saveDefaultLocation(default_directory);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::onSelectFile(QPointer<QLineEdit> _line_edit)
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle("Select File");
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(sight::ui::dialog::location::READ);
    dialog_file.setType(sight::ui::dialog::location::SINGLE_FILE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        _line_edit->setText(QString::fromStdString(result->get_file().string()));
        dialog_file.saveDefaultLocation(default_directory);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
