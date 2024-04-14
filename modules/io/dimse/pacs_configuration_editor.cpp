/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "pacs_configuration_editor.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slots.hxx>

#include <io/dimse/exceptions/base.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace sight::module::io::dimse
{

static const core::com::slots::key_t SHOW_DIALOG_SLOT = "showDialog";

static const service::base::key_t SHOW_DIALOG_CONFIG = "showDialog";

//------------------------------------------------------------------------------

pacs_configuration_editor::pacs_configuration_editor() noexcept :
    sight::service::notifier(m_signals)
{
    m_slot_show_dialog = this->new_slot(SHOW_DIALOG_SLOT, &pacs_configuration_editor::show_dialog);
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::configuring()
{
    sight::ui::service::initialize();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_show_dialog = config->get<bool>(SHOW_DIALOG_CONFIG, m_show_dialog);
    }
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::starting()
{
    // Create the worker.
    m_request_worker = core::thread::worker::make();

    const auto pacs_configuration = m_config.lock();

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* grid_layout = new QGridLayout();

    m_scu_app_entity_title_edit = new QLineEdit();
    m_scu_app_entity_title_edit->setText(pacs_configuration->get_local_application_title().c_str());
    auto* const ae_tof_scu = new QLabel("AET of the SCU:");
    ae_tof_scu->setToolTip("Application entity title of the client");
    ae_tof_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(ae_tof_scu, 0, 0);
    grid_layout->addWidget(m_scu_app_entity_title_edit, 0, 1);

    m_scp_app_entity_title_edit = new QLineEdit();
    m_scp_app_entity_title_edit->setText(pacs_configuration->get_pacs_application_title().c_str());
    auto* const ae_tof_scp = new QLabel("AET of the SCP:");
    ae_tof_scp->setToolTip("Application entity title of the PACS server");
    ae_tof_scp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(ae_tof_scp, 1, 0);
    grid_layout->addWidget(m_scp_app_entity_title_edit, 1, 1);

    m_scp_host_name_edit = new QLineEdit();
    m_scp_host_name_edit->setText(pacs_configuration->get_pacs_host_name().c_str());
    auto* const host_name_of_scp = new QLabel("Host name of the SCP:");
    host_name_of_scp->setToolTip("Host name of the PACS server");
    host_name_of_scp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(host_name_of_scp, 2, 0);
    grid_layout->addWidget(m_scp_host_name_edit, 2, 1);

    m_scp_port_edit = new QSpinBox();
    m_scp_port_edit->setRange(0, 65535);
    m_scp_port_edit->setValue(pacs_configuration->get_pacs_application_port());
    auto* const scp_port = new QLabel("Port of the SCP");
    scp_port->setToolTip("Port of the PACS server");
    scp_port->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(scp_port, 3, 0);
    grid_layout->addWidget(m_scp_port_edit, 3, 1);

    m_move_app_entity_title_edit = new QLineEdit();
    m_move_app_entity_title_edit->setText(pacs_configuration->get_move_application_title().c_str());
    auto* const aet_of_move_scu = new QLabel("AET of the move SCU");
    aet_of_move_scu->setToolTip("Application entity title of the move client");
    aet_of_move_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(aet_of_move_scu, 4, 0);
    grid_layout->addWidget(m_move_app_entity_title_edit, 4, 1);

    m_move_port = new QSpinBox();
    m_move_port->setRange(0, 65535);
    m_move_port->setValue(pacs_configuration->get_move_application_port());
    auto* const port_of_move_scu = new QLabel("Port of the move SCU:");
    port_of_move_scu->setToolTip("Port of the move client");
    port_of_move_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(port_of_move_scu, 5, 0);
    grid_layout->addWidget(m_move_port, 5, 1);

    m_retrieve_method_widget = new QComboBox();
    m_retrieve_method_widget->addItem("Move");
    m_retrieve_method_widget->addItem("Get");
    m_retrieve_method_widget->setCurrentIndex(
        (pacs_configuration->get_retrieve_method()
         == sight::io::dimse::data::pacs_configuration::retrieve_method::move) ? 0 : 1
    );
    auto* const retrieve_method = new QLabel("Retrieve method:");
    retrieve_method->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(retrieve_method, 6, 0);
    grid_layout->addWidget(m_retrieve_method_widget, 6, 1);

    m_ping_pacs_button_widget = new QPushButton("Ping PACS");
    grid_layout->addWidget(m_ping_pacs_button_widget, 7, 0, 1, 2);

    qt_container->set_layout(grid_layout);

    // Connect signals.
    QObject::connect(m_ping_pacs_button_widget, &QPushButton::clicked, this, &self_t::ping_pacs);
    QObject::connect(
        m_scu_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_scu_app_entity_title_changed
    );
    QObject::connect(m_scp_host_name_edit, &QLineEdit::editingFinished, this, &self_t::on_scp_host_name_changed);
    QObject::connect(
        m_scp_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_scp_app_entity_title_changed
    );
    QObject::connect(
        m_scp_port_edit,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &self_t::on_scp_port_changed
    );
    QObject::connect(
        m_move_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_move_app_entity_title_changed
    );
    QObject::connect(
        m_move_port,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &self_t::on_move_port_changed
    );
    QObject::connect(
        m_retrieve_method_widget,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &self_t::on_retrieve_method_changed
    );
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::updating()
{
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::stopping()
{
    // Disconnect signals.
    QObject::disconnect(m_ping_pacs_button_widget, &QPushButton::clicked, this, &self_t::ping_pacs);
    QObject::disconnect(
        m_scu_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_scu_app_entity_title_changed
    );
    QObject::disconnect(
        m_scp_host_name_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_scp_host_name_changed
    );
    QObject::disconnect(
        m_scp_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_scp_app_entity_title_changed
    );
    QObject::disconnect(
        m_scp_port_edit,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &self_t::on_scp_port_changed
    );
    QObject::disconnect(
        m_move_app_entity_title_edit,
        &QLineEdit::editingFinished,
        this,
        &self_t::on_move_app_entity_title_changed
    );
    QObject::disconnect(
        m_move_port,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &self_t::on_move_port_changed
    );
    QObject::disconnect(
        m_retrieve_method_widget,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &self_t::on_retrieve_method_changed
    );

    // Stop the worker.
    m_request_worker->stop();
    m_request_worker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::ping_pacs()
{
    m_request_worker->post(
        [&]
        {
            const auto pacs_configuration = m_config.lock();

            auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

            bool success = false;
            try
            {
                series_enquirer->initialize(
                    pacs_configuration->get_local_application_title(),
                    pacs_configuration->get_pacs_host_name(),
                    pacs_configuration->get_pacs_application_port(),
                    pacs_configuration->get_pacs_application_title()
                );
                series_enquirer->connect();
                success = series_enquirer->ping_pacs();
            }
            catch(sight::io::dimse::exceptions::base& e)
            {
                SIGHT_ERROR("Can't establish a connection with the PACS: " + std::string(e.what()));
            }

            if(series_enquirer->is_connected_to_pacs())
            {
                series_enquirer->disconnect();
            }

            // Display a message with the ping result.
            if(m_show_dialog)
            {
                if(success)
                {
                    m_slot_show_dialog->async_run("Ping Pacs", "Ping succeeded!");
                }
                else
                {
                    m_slot_show_dialog->async_run("Ping Pacs", "Ping failed!");
                }
            }

            if(success)
            {
                this->notifier::info("Ping succeeded!");
                SIGHT_INFO("Ping succeeded")
            }
            else
            {
                this->notifier::failure("Ping failed!");
                SIGHT_INFO("Ping failed")
            }
        });
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::modified_notify(sight::io::dimse::data::pacs_configuration::sptr _pacs_configuration)
{
    auto sig = _pacs_configuration->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_scu_app_entity_title_changed()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_local_application_title(m_scu_app_entity_title_edit->text().toStdString());

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_scp_host_name_changed()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_pacs_host_name(m_scp_host_name_edit->text().toStdString());

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_scp_app_entity_title_changed()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_pacs_application_title(m_scp_app_entity_title_edit->text().toStdString());

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_scp_port_changed(int _value)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_pacs_application_port(static_cast<std::uint16_t>(_value));

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_move_app_entity_title_changed()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_move_application_title(m_move_app_entity_title_edit->text().toStdString());

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_move_port_changed(int _value)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_move_application_port(static_cast<std::uint16_t>(_value));

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::on_retrieve_method_changed(int _index)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->set_retrieve_method(
        (_index == 0)
        ? (sight::io::dimse::data::pacs_configuration::retrieve_method::move)
        : (sight::io::dimse::data::pacs_configuration::retrieve_method::get)
    );

    this->modified_notify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::show_dialog(const std::string _title, const std::string _message)
{
    sight::ui::dialog::message message_box;
    message_box.set_title(_title);
    message_box.set_message(_message);
    message_box.set_icon(ui::dialog::message::info);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

} // namespace sight::module::io::dimse
