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

#include <io/dimse/exceptions/Base.hpp>

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

static const service::base::key_t s_SHOW_DIALOG_CONFIG = "showDialog";

//------------------------------------------------------------------------------

pacs_configuration_editor::pacs_configuration_editor() noexcept :
    sight::service::notifier(m_signals)
{
    m_slotShowDialog = this->new_slot(SHOW_DIALOG_SLOT, &pacs_configuration_editor::showDialog);
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::configuring()
{
    sight::ui::service::initialize();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_showDialog = config->get<bool>(s_SHOW_DIALOG_CONFIG, m_showDialog);
    }
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::worker::make();

    const auto pacs_configuration = m_config.lock();

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* grid_layout = new QGridLayout();

    m_SCUAppEntityTitleEdit = new QLineEdit();
    m_SCUAppEntityTitleEdit->setText(pacs_configuration->getLocalApplicationTitle().c_str());
    auto* const ae_tof_scu = new QLabel("AET of the SCU:");
    ae_tof_scu->setToolTip("Application entity title of the client");
    ae_tof_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(ae_tof_scu, 0, 0);
    grid_layout->addWidget(m_SCUAppEntityTitleEdit, 0, 1);

    m_SCPAppEntityTitleEdit = new QLineEdit();
    m_SCPAppEntityTitleEdit->setText(pacs_configuration->getPacsApplicationTitle().c_str());
    auto* const ae_tof_scp = new QLabel("AET of the SCP:");
    ae_tof_scp->setToolTip("Application entity title of the PACS server");
    ae_tof_scp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(ae_tof_scp, 1, 0);
    grid_layout->addWidget(m_SCPAppEntityTitleEdit, 1, 1);

    m_SCPHostNameEdit = new QLineEdit();
    m_SCPHostNameEdit->setText(pacs_configuration->getPacsHostName().c_str());
    auto* const host_name_of_scp = new QLabel("Host name of the SCP:");
    host_name_of_scp->setToolTip("Host name of the PACS server");
    host_name_of_scp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(host_name_of_scp, 2, 0);
    grid_layout->addWidget(m_SCPHostNameEdit, 2, 1);

    m_SCPPortEdit = new QSpinBox();
    m_SCPPortEdit->setRange(0, 65535);
    m_SCPPortEdit->setValue(pacs_configuration->getPacsApplicationPort());
    auto* const scp_port = new QLabel("Port of the SCP");
    scp_port->setToolTip("Port of the PACS server");
    scp_port->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(scp_port, 3, 0);
    grid_layout->addWidget(m_SCPPortEdit, 3, 1);

    m_moveAppEntityTitleEdit = new QLineEdit();
    m_moveAppEntityTitleEdit->setText(pacs_configuration->getMoveApplicationTitle().c_str());
    auto* const aet_of_move_scu = new QLabel("AET of the move SCU");
    aet_of_move_scu->setToolTip("Application entity title of the move client");
    aet_of_move_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(aet_of_move_scu, 4, 0);
    grid_layout->addWidget(m_moveAppEntityTitleEdit, 4, 1);

    m_movePort = new QSpinBox();
    m_movePort->setRange(0, 65535);
    m_movePort->setValue(pacs_configuration->getMoveApplicationPort());
    auto* const port_of_move_scu = new QLabel("Port of the move SCU:");
    port_of_move_scu->setToolTip("Port of the move client");
    port_of_move_scu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(port_of_move_scu, 5, 0);
    grid_layout->addWidget(m_movePort, 5, 1);

    m_retrieveMethodWidget = new QComboBox();
    m_retrieveMethodWidget->addItem("Move");
    m_retrieveMethodWidget->addItem("Get");
    m_retrieveMethodWidget->setCurrentIndex(
        (pacs_configuration->getRetrieveMethod()
         == sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD) ? 0 : 1
    );
    auto* const retrieve_method = new QLabel("Retrieve method:");
    retrieve_method->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid_layout->addWidget(retrieve_method, 6, 0);
    grid_layout->addWidget(m_retrieveMethodWidget, 6, 1);

    m_pingPacsButtonWidget = new QPushButton("Ping PACS");
    grid_layout->addWidget(m_pingPacsButtonWidget, 7, 0, 1, 2);

    qt_container->setLayout(grid_layout);

    // Connect signals.
    QObject::connect(m_pingPacsButtonWidget, SIGNAL(clicked()), this, SLOT(pingPACS()));
    QObject::connect(
        m_SCUAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(
            onSCUAppEntityTitleChanged()
        )
    );
    QObject::connect(m_SCPHostNameEdit,SIGNAL(editingFinished()),this,SLOT(onSCPHostNameChanged()));
    QObject::connect(
        m_SCPAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onSCPAppEntityTitleChanged())
    );
    QObject::connect(
        m_SCPPortEdit,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onSCPPortChanged(int))
    );
    QObject::connect(
        m_moveAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onMoveAppEntityTitleChanged())
    );
    QObject::connect(
        m_movePort,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onMovePortChanged(int))
    );
    QObject::connect(
        m_retrieveMethodWidget,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(onRetrieveMethodChanged(int))
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
    QObject::disconnect(m_pingPacsButtonWidget,SIGNAL(clicked()),this,SLOT(pingPACS()));
    QObject::disconnect(
        m_SCUAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onSCUAppEntityTitleChanged())
    );
    QObject::disconnect(
        m_SCPHostNameEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onSCPHostNameChanged())
    );
    QObject::disconnect(
        m_SCPAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onSCPAppEntityTitleChanged())
    );
    QObject::disconnect(
        m_SCPPortEdit,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onSCPPortChanged(int))
    );
    QObject::disconnect(
        m_moveAppEntityTitleEdit,
        SIGNAL(editingFinished()),
        this,
        SLOT(onMoveAppEntityTitleChanged())
    );
    QObject::disconnect(
        m_movePort,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onMovePortChanged(int))
    );
    QObject::disconnect(
        m_retrieveMethodWidget,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(onRetrieveMethodChanged(int))
    );

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::pingPACS()
{
    m_requestWorker->post(
        [&]
        {
            const auto pacs_configuration = m_config.lock();

            auto series_enquirer = std::make_shared<sight::io::dimse::SeriesEnquirer>();

            bool success = false;
            try
            {
                series_enquirer->initialize(
                    pacs_configuration->getLocalApplicationTitle(),
                    pacs_configuration->getPacsHostName(),
                    pacs_configuration->getPacsApplicationPort(),
                    pacs_configuration->getPacsApplicationTitle()
                );
                series_enquirer->connect();
                success = series_enquirer->pingPacs();
            }
            catch(sight::io::dimse::exceptions::Base& e)
            {
                SIGHT_ERROR("Can't establish a connection with the PACS: " + std::string(e.what()));
            }

            if(series_enquirer->isConnectedToPacs())
            {
                series_enquirer->disconnect();
            }

            // Display a message with the ping result.
            if(m_showDialog)
            {
                if(success)
                {
                    m_slotShowDialog->async_run("Ping Pacs","Ping succeeded!");
                }
                else
                {
                    m_slotShowDialog->async_run("Ping Pacs","Ping failed!");
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

void pacs_configuration_editor::modifiedNotify(sight::io::dimse::data::PacsConfiguration::sptr _pacs_configuration)
{
    auto sig = _pacs_configuration->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onSCUAppEntityTitleChanged()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setLocalApplicationTitle(m_SCUAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onSCPHostNameChanged()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setPacsHostName(m_SCPHostNameEdit->text().toStdString());

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onSCPAppEntityTitleChanged()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setPacsApplicationTitle(m_SCPAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onSCPPortChanged(int _value)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setPacsApplicationPort(static_cast<std::uint16_t>(_value));

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onMoveAppEntityTitleChanged()
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setMoveApplicationTitle(m_moveAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onMovePortChanged(int _value)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setMoveApplicationPort(static_cast<std::uint16_t>(_value));

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::onRetrieveMethodChanged(int _index)
{
    const auto pacs_configuration = m_config.lock();

    pacs_configuration->setRetrieveMethod(
        (_index
         == 0) ? (sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD) : (sight::io::dimse::data::
                                                                                      PacsConfiguration::
                                                                                      GET_RETRIEVE_METHOD)
    );

    this->modifiedNotify(pacs_configuration.get_shared());
}

//------------------------------------------------------------------------------

void pacs_configuration_editor::showDialog(const std::string _title,const std::string _message)
{
    sight::ui::dialog::message message_box;
    message_box.setTitle(_title);
    message_box.setMessage(_message);
    message_box.setIcon(ui::dialog::message::INFO);
    message_box.addButton(ui::dialog::message::OK);
    message_box.show();
}

} // namespace sight::module::io::dimse
