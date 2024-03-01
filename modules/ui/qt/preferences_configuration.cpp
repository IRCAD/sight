/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
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
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
namespace sight::module::ui::qt
{

const core::com::signals::key_t preferences_configuration::PARAMETERS_MODIFIED_SIG = "parameters_modified";
const core::com::signals::key_t preferences_configuration::PREFERENCE_CHANGED_SIG  = "preference_changed";

//------------------------------------------------------------------------------

sight::ui::parameter_t preferences_configuration::convert_value(const preference_elt& _elt)
{
    switch(_elt.m_type)
    {
        case preference_t::text:
        case preference_t::path:
        case preference_t::combobox:
        case preference_t::file:
        {
            return _elt.m_preference_value;
        }

        case preference_t::INT:
        {
            return std::stoi(_elt.m_preference_value);
        }

        case preference_t::DOUBLE:
        {
            return std::stod(_elt.m_preference_value);
        }

        case preference_t::checkbox:
        {
            return _elt.m_preference_value == "true";
        }

        case preference_t::list:
        {
            std::vector<std::string> list;
            boost::split(list, _elt.m_preference_value, boost::is_any_of(_elt.m_separator));
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
    m_sig_parameters_modified = new_signal<parameters_modified_signal_t>(PARAMETERS_MODIFIED_SIG);
    m_sig_preference_changed  = new_signal<changed_signal_t>(PREFERENCE_CHANGED_SIG);

    new_slot(slots::REQUEST_VALUES, &preferences_configuration::request_values, this);
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
        preference_elt pref;

        auto type = cfg.second.get<std::string>("type"); //  balise type of .xml preference
        if(type == "checkbox")
        {
            pref.m_type = preference_t::checkbox;
        }
        else if(type == "text")
        {
            pref.m_type = preference_t::text;
        }
        else if(type == "path")
        {
            pref.m_type = preference_t::path;
        }
        else if(type == "file")
        {
            pref.m_type = preference_t::file;
        }
        else if(type == "combobox")
        {
            pref.m_type = preference_t::combobox;

            const auto values_cfg = cfg.second.get<std::string>("values");
            const boost::char_separator<char> sep(", ;");
            const boost::tokenizer<boost::char_separator<char> > tokens {values_cfg, sep};

            for(const std::string& value : tokens)
            {
                pref.m_values.push_back(value);
            }
        }
        else if(type == "double")
        {
            pref.m_type = preference_t::DOUBLE;

            pref.m_d_min_max.first  = cfg.second.get<double>("min", pref.m_d_min_max.first);
            pref.m_d_min_max.second = cfg.second.get<double>("max", pref.m_d_min_max.second);
        }
        else if(type == "int")
        {
            pref.m_type = preference_t::INT;

            pref.m_i_min_max.first  = cfg.second.get<int>("min", pref.m_i_min_max.first);
            pref.m_i_min_max.second = cfg.second.get<int>("max", pref.m_i_min_max.second);
        }
        else if(type == "list")
        {
            pref.m_type = preference_t::list;

            pref.m_separator = cfg.second.get("separator", ",");
        }
        else
        {
            SIGHT_ERROR("Preference type " << type << " is not implemented");
        }

        pref.m_name           = cfg.second.get<std::string>("name");
        pref.m_preference_key = cfg.second.get<std::string>("key");
        pref.m_default_value  = cfg.second.get<std::string>("default_value");

        m_preferences.emplace_back(pref);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::starting()
{
    this->action_service_starting();
}

//------------------------------------------------------------------------------

void preferences_configuration::updating()
{
    // Preferences may have been updated after start or last update.
    this->update_from_preferences();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QDialog dialog;
    dialog.setObjectName("preferences_configuration");
    QPointer<QGridLayout> layout = new QGridLayout();

    int index = 0;
    for(preference_elt& pref : m_preferences)
    {
        QPointer<QLabel> label = new QLabel(QString::fromStdString(pref.m_name));
        layout->addWidget(label, index, 0);
        QWidget* widget_to_reset = nullptr;

        if(pref.m_type == preference_t::text || pref.m_type == preference_t::list)
        {
            pref.m_line_edit = new QLineEdit();
            pref.m_line_edit->setObjectName(pref.m_preference_key.c_str());
            pref.m_line_edit->setText(QString::fromStdString(pref.m_preference_value));
            layout->addWidget(pref.m_line_edit, index, 1);
            widget_to_reset = pref.m_line_edit;
        }
        else if(pref.m_type == preference_t::checkbox)
        {
            pref.m_check_box = new QCheckBox();
            pref.m_check_box->setObjectName(pref.m_preference_key.c_str());
            pref.m_check_box->setChecked(pref.m_preference_value == "true");
            layout->addWidget(pref.m_check_box, index, 1);
            widget_to_reset = pref.m_check_box;
        }
        else if(pref.m_type == preference_t::INT || pref.m_type == preference_t::DOUBLE)
        {
            pref.m_line_edit = new QLineEdit();

            if(pref.m_type == preference_t::INT)
            {
                pref.m_line_edit->setValidator(new QIntValidator(pref.m_i_min_max.first, pref.m_i_min_max.second));
            }
            else
            {
                pref.m_line_edit->setValidator(
                    new QDoubleValidator(
                        pref.m_d_min_max.first,
                        pref.m_d_min_max.second,
                        6
                    )
                );
            }

            pref.m_line_edit->setObjectName(pref.m_preference_key.c_str());
            pref.m_line_edit->setText(QString::fromStdString(pref.m_preference_value));

            layout->addWidget(pref.m_line_edit, index, 1);
            widget_to_reset = pref.m_line_edit;
            QObject::connect(
                pref.m_line_edit,
                &QLineEdit::textEdited,
                [&]()
                {
                    int pos               = 0;
                    QLineEdit* const edit = pref.m_line_edit;
                    QString text          = edit->text();
                    const bool is_valid   = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                    if(qApp->styleSheet().isEmpty())
                    {
                        static const QColor s_DEFAULT_TEXT_COLOR = QLineEdit().palette().color(QPalette::Text);

                        QPalette palette   = edit->palette();
                        const QColor color = is_valid ? s_DEFAULT_TEXT_COLOR : QColorConstants::Red;
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
        else if(pref.m_type == preference_t::path)
        {
            pref.m_line_edit = new QLineEdit();
            pref.m_line_edit->setObjectName(pref.m_preference_key.c_str());
            pref.m_line_edit->setText(QString::fromStdString(pref.m_preference_value));
            layout->addWidget(pref.m_line_edit, index, 1);
            widget_to_reset = pref.m_line_edit;
            QPointer<QPushButton> directory_selector = new QPushButton("...");
            layout->addWidget(directory_selector, index, 2);
            QObject::connect(
                directory_selector.data(),
                &QPushButton::clicked,
                [pref]()
                {
                    sight::module::ui::qt::preferences_configuration::on_select_dir(pref.m_line_edit);
                });
        }
        else if(pref.m_type == preference_t::file)
        {
            pref.m_line_edit = new QLineEdit();
            pref.m_line_edit->setObjectName(pref.m_preference_key.c_str());
            pref.m_line_edit->setText(QString::fromStdString(pref.m_preference_value));
            layout->addWidget(pref.m_line_edit, index, 1);
            widget_to_reset = pref.m_line_edit;
            QPointer<QPushButton> directory_selector = new QPushButton("...");
            layout->addWidget(directory_selector, index, 2);
            QObject::connect(
                directory_selector.data(),
                &QPushButton::clicked,
                [pref]()
                {
                    sight::module::ui::qt::preferences_configuration::on_select_file(pref.m_line_edit);
                });
        }
        else if(pref.m_type == preference_t::combobox)
        {
            pref.m_combo_box = new QComboBox();
            pref.m_combo_box->setObjectName(pref.m_preference_key.c_str());
            for(const std::string& value : pref.m_values)
            {
                pref.m_combo_box->addItem(QString::fromStdString(value));
            }

            const int current_index = pref.m_combo_box->findText(QString::fromStdString(pref.m_preference_value));
            if(current_index < 0)
            {
                SIGHT_WARN(
                    "Preference '" + pref.m_preference_value
                    + "' can't be find in combobox. The first one is selected."
                );
                pref.m_combo_box->setCurrentIndex(0);
            }
            else
            {
                pref.m_combo_box->setCurrentIndex(current_index);
            }

            layout->addWidget(pref.m_combo_box, index, 1);
            widget_to_reset = pref.m_combo_box;
        }

        QPointer<QPushButton> default_selector = new QPushButton("R");

        default_selector->setFocusPolicy(Qt::NoFocus);
        default_selector->setToolTip("Reset this preference value to default");
        default_selector->setMaximumWidth(30);
        layout->addWidget(default_selector, index, 3);

        if(widget_to_reset != nullptr)
        {
            QObject::connect(
                default_selector,
                &QPushButton::clicked,
                this,
                [this, widget_to_reset]()
                {
                    this->on_reset_to_default_value(widget_to_reset);
                });
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

    QObject::connect(cancel_button.data(), &QPushButton::clicked, &dialog, &QDialog::reject);
    QObject::connect(ok_button.data(), &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.setLayout(layout);

    if(dialog.exec() == QDialog::Accepted)
    {
        sight::ui::preferences preferences;

        for(preference_elt& pref : m_preferences)
        {
            bool preference_update = false; // only emit signal for preference that has changed.

            if((pref.m_type == preference_t::text || pref.m_type == preference_t::path
                || pref.m_type == preference_t::file || pref.m_type == preference_t::list)
               && !pref.m_line_edit->text().isEmpty())
            {
                preference_update =
                    pref.m_preference_value != pref.m_line_edit->text().toStdString();
                pref.m_preference_value = pref.m_line_edit->text().toStdString();
            }
            else if(pref.m_type == preference_t::checkbox)
            {
                const std::string checked = pref.m_check_box->isChecked() ? "true" : "false";
                preference_update       = pref.m_preference_value != checked;
                pref.m_preference_value = checked;
            }
            else if(pref.m_type == preference_t::INT || pref.m_type == preference_t::DOUBLE)
            {
                int pos               = 0;
                QLineEdit* const edit = pref.m_line_edit;
                QString text          = edit->text();
                const bool is_valid   = edit->validator()->validate(text, pos) == QValidator::State::Acceptable;

                if(is_valid)
                {
                    preference_update =
                        pref.m_preference_value != pref.m_line_edit->text().toStdString();
                    pref.m_preference_value = pref.m_line_edit->text().toStdString();
                }

                if(qApp->styleSheet().isEmpty())
                {
                    static const QColor s_DEFAULT_TEXT_COLOR = QLineEdit().palette().color(QPalette::Text);

                    QPalette palette = edit->palette();
                    palette.setColor(QPalette::Text, s_DEFAULT_TEXT_COLOR);
                }
                else
                {
                    edit->setProperty("type", "");
                    edit->style()->unpolish(edit);
                    edit->style()->polish(edit);
                }
            }
            else if(pref.m_type == preference_t::combobox)
            {
                preference_update =
                    pref.m_preference_value != pref.m_combo_box->currentText().toStdString();
                pref.m_preference_value = pref.m_combo_box->currentText().toStdString();
            }

            preferences.put(pref.m_preference_key, pref.m_preference_value);

            // Emit preferenceChanged signal with new value and preference key.
            if(preference_update)
            {
                const auto value = this->convert_value(pref);
                m_sig_preference_changed->async_emit(value, pref.m_preference_key);
            }
        }

        m_sig_parameters_modified->async_emit();
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::stopping()
{
    this->action_service_stopping();
}

//------------------------------------------------------------------------------

void preferences_configuration::request_values()
{
    this->update_from_preferences();

    for(auto& pref : m_preferences)
    {
        const auto value = this->convert_value(pref);
        m_sig_preference_changed->async_emit(value, pref.m_preference_key);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::on_select_dir(QPointer<QLineEdit> _line_edit)
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title("Select Storage directory");
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(sight::ui::dialog::location::write);
    dialog_file.set_type(sight::ui::dialog::location::folder);

    const auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_folder());
        _line_edit->setText(QString::fromStdString(result->get_folder().string()));
        dialog_file.save_default_location(default_directory);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::on_select_file(QPointer<QLineEdit> _line_edit)
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title("Select File");
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(sight::ui::dialog::location::read);
    dialog_file.set_type(sight::ui::dialog::location::single_file);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        _line_edit->setText(QString::fromStdString(result->get_file().string()));
        dialog_file.save_default_location(default_directory);
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::update_from_preferences() noexcept
{
    try
    {
        sight::ui::preferences preferences;

        for(auto& preference : m_preferences)
        {
            if(const auto& found = preferences.get_optional<std::string>(preference.m_preference_key); found)
            {
                preference.m_preference_value = *found;
            }
            else
            {
                preference.m_preference_value = preference.m_default_value;
                preferences.put(preference.m_preference_key, preference.m_default_value);
            }
        }
    }
    catch(const sight::ui::preferences_disabled& /*e*/)
    {
        // Nothing to do...
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::on_reset_to_default_value(QObject* _widget)
{
    if(auto* const line_edit = qobject_cast<QLineEdit*>(_widget); line_edit != nullptr)
    {
        for(const auto& pref : m_preferences)
        {
            if(pref.m_line_edit == line_edit)
            {
                QString default_value;

                switch(pref.m_type)
                {
                    case preference_t::text:
                    case preference_t::path:
                    case preference_t::file:
                    case preference_t::list:
                    case preference_t::INT:
                    case preference_t::DOUBLE:
                        default_value = QString::fromStdString(pref.m_default_value);
                        break;

                    default:
                        break;
                }

                line_edit->setText(default_value);

                const sight::ui::parameter_t variant_value = default_value.toStdString();
                auto sig                                   = this->signal<changed_signal_t>(PREFERENCE_CHANGED_SIG);
                sig->async_emit(variant_value, pref.m_preference_key);

                break;
            }
        }
    }
    else if(auto* const combo_box = qobject_cast<QComboBox*>(_widget); combo_box != nullptr)
    {
        for(const auto& pref : m_preferences)
        {
            if(pref.m_combo_box == combo_box)
            {
                const QString default_value = QString::fromStdString(pref.m_default_value);
                int index                   = combo_box->findText(default_value);
                if(index != -1)
                {
                    combo_box->setCurrentIndex(index);
                }

                const sight::ui::parameter_t variant_value = default_value.toStdString();
                auto sig                                   = this->signal<changed_signal_t>(PREFERENCE_CHANGED_SIG);
                sig->async_emit(variant_value, pref.m_preference_key);

                break;
            }
        }
    }
    else if(auto* const check_box = qobject_cast<QCheckBox*>(_widget); check_box != nullptr)
    {
        for(const auto& pref : m_preferences)
        {
            if(pref.m_check_box == check_box)
            {
                const bool check_state = pref.m_default_value == "true";
                check_box->setChecked(check_state);

                const sight::ui::parameter_t variant_value = check_state;
                auto sig                                   = this->signal<changed_signal_t>(PREFERENCE_CHANGED_SIG);
                sig->async_emit(variant_value, pref.m_preference_key);

                break;
            }
        }
    }
}

} // namespace sight::module::ui::qt
