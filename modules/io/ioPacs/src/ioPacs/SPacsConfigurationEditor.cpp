/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioPacs/SPacsConfigurationEditor.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slots.hxx>

#include <fwPacsIO/exceptions/Base.hpp>

#include <gui/dialog/MessageDialog.hpp>

#include <guiQt/container/QtContainer.hpp>

#include <services/macros.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace ioPacs
{

static const core::com::Slots::SlotKeyType s_SHOW_DIALOG_SLOT = "showDiaog";

static const services::IService::KeyType s_SHOW_DIALOG_CONFIG = "showDialog";

static const services::IService::KeyType s_CONFIG_INOUT = "config";

//------------------------------------------------------------------------------

SPacsConfigurationEditor::SPacsConfigurationEditor() noexcept
{
    m_slotShowDialog = this->newSlot(s_SHOW_DIALOG_SLOT, &SPacsConfigurationEditor::showDialog, this);
}

//------------------------------------------------------------------------------

SPacsConfigurationEditor::~SPacsConfigurationEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::configuring()
{
    gui::IGuiContainerSrv::initialize();

    const ConfigType configType = this->getConfigTree();
    const auto config           = configType.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_showDialog = config->get< bool >(s_SHOW_DIALOG_CONFIG, m_showDialog);
    }
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::Worker::New();

    const auto pacsConfiguration = this->getLockedInOut< const ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    gui::IGuiContainerSrv::create();
    guiQt::container::QtContainer::sptr qtContainer = guiQt::container::QtContainer::dynamicCast(getContainer());

    QGridLayout* gridLayout = new QGridLayout();

    m_SCUAppEntityTitleEdit = new QLineEdit();
    m_SCUAppEntityTitleEdit->setText(pacsConfiguration->getLocalApplicationTitle().c_str());
    QLabel* const AETofSCU = new QLabel("AET of the SCU:");
    AETofSCU->setToolTip("Application entity title of the client");
    AETofSCU->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(AETofSCU, 0, 0);
    gridLayout->addWidget(m_SCUAppEntityTitleEdit, 0, 1);

    m_SCPAppEntityTitleEdit = new QLineEdit();
    m_SCPAppEntityTitleEdit->setText(pacsConfiguration->getPacsApplicationTitle().c_str());
    QLabel* const AETofSCP = new QLabel("AET of the SCP:");
    AETofSCP->setToolTip("Application entity title of the PACS server");
    AETofSCP->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(AETofSCP, 1, 0);
    gridLayout->addWidget(m_SCPAppEntityTitleEdit, 1, 1);

    m_SCPHostNameEdit = new QLineEdit();
    m_SCPHostNameEdit->setText(pacsConfiguration->getPacsHostName().c_str());
    QLabel* const hostNameOfSCP = new QLabel("Host name of the SCP:");
    hostNameOfSCP->setToolTip("Host name of the PACS server");
    hostNameOfSCP->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(hostNameOfSCP, 2, 0);
    gridLayout->addWidget(m_SCPHostNameEdit, 2, 1);

    m_SCPPortEdit = new QSpinBox();
    m_SCPPortEdit->setRange(0, 65535);
    m_SCPPortEdit->setValue(pacsConfiguration->getPacsApplicationPort());
    QLabel* const SCPPort = new QLabel("Port of the SCP");
    SCPPort->setToolTip("Port of the PACS server");
    SCPPort->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(SCPPort, 3, 0);
    gridLayout->addWidget(m_SCPPortEdit, 3, 1);

    m_moveAppEntityTitleEdit = new QLineEdit();
    m_moveAppEntityTitleEdit->setText(pacsConfiguration->getMoveApplicationTitle().c_str());
    QLabel* const AETOfMoveSCU = new QLabel("AET of the move SCU");
    AETOfMoveSCU->setToolTip("Application entity title of the move client");
    AETOfMoveSCU->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(AETOfMoveSCU, 4, 0);
    gridLayout->addWidget(m_moveAppEntityTitleEdit, 4, 1);

    m_movePort = new QSpinBox();
    m_movePort->setRange(0, 65535);
    m_movePort->setValue(pacsConfiguration->getMoveApplicationPort());
    QLabel* const PortOfMoveSCU = new QLabel("Port of the move SCU:");
    PortOfMoveSCU->setToolTip("Port of the move client");
    PortOfMoveSCU->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(PortOfMoveSCU, 5, 0);
    gridLayout->addWidget(m_movePort, 5, 1);

    m_retrieveMethodWidget = new QComboBox();
    m_retrieveMethodWidget->addItem("Move");
    m_retrieveMethodWidget->addItem("Get");
    m_retrieveMethodWidget->setCurrentIndex(
        (pacsConfiguration->getRetrieveMethod() == ::fwPacsIOdata::PacsConfiguration::MOVE_RETRIEVE_METHOD) ? 0 : 1);
    QLabel* const RetrieveMethod = new QLabel("Retrieve method:");
    RetrieveMethod->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(RetrieveMethod, 6, 0);
    gridLayout->addWidget(m_retrieveMethodWidget, 6, 1);

    m_pingPacsButtonWidget = new QPushButton("Ping PACS");
    gridLayout->addWidget(m_pingPacsButtonWidget, 7, 0, 1, 2);

    qtContainer->setLayout(gridLayout);

    // Connect signals.
    QObject::connect(m_pingPacsButtonWidget, SIGNAL(clicked()), this, SLOT(pingPACS()));
    QObject::connect(m_SCUAppEntityTitleEdit, SIGNAL(editingFinished()), this, SLOT(
                         onSCUAppEntityTitleChanged()));
    QObject::connect(m_SCPHostNameEdit, SIGNAL(editingFinished()), this, SLOT(onSCPHostNameChanged()));
    QObject::connect(m_SCPAppEntityTitleEdit, SIGNAL(editingFinished()), this,
                     SLOT(onSCPAppEntityTitleChanged()));
    QObject::connect(m_SCPPortEdit, SIGNAL(valueChanged(int)), this,
                     SLOT(onSCPPortChanged(int)));
    QObject::connect(m_moveAppEntityTitleEdit, SIGNAL(editingFinished()), this,
                     SLOT(onMoveAppEntityTitleChanged()));
    QObject::connect(m_movePort, SIGNAL(valueChanged(int)), this,
                     SLOT(onMovePortChanged(int)));
    QObject::connect(m_retrieveMethodWidget, SIGNAL(currentIndexChanged(int)), this,
                     SLOT(onRetrieveMethodChanged(int)));

}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::updating()
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::stopping()
{
    // Disconnect signals.
    QObject::disconnect(m_pingPacsButtonWidget, SIGNAL(clicked()), this, SLOT(pingPACS()));
    QObject::disconnect(m_SCUAppEntityTitleEdit, SIGNAL(editingFinished()), this,
                        SLOT(onSCUAppEntityTitleChanged()));
    QObject::disconnect(m_SCPHostNameEdit, SIGNAL(editingFinished()), this,
                        SLOT(onSCPHostNameChanged()));
    QObject::disconnect(m_SCPAppEntityTitleEdit, SIGNAL(editingFinished()), this,
                        SLOT(onSCPAppEntityTitleChanged()));
    QObject::disconnect(m_SCPPortEdit, SIGNAL(valueChanged(int)), this,
                        SLOT(onSCPPortChanged(int)));
    QObject::disconnect(m_moveAppEntityTitleEdit, SIGNAL(editingFinished()), this,
                        SLOT(onMoveAppEntityTitleChanged()));
    QObject::disconnect(m_movePort, SIGNAL(valueChanged(int)), this,
                        SLOT(onMovePortChanged(int)));
    QObject::disconnect(m_retrieveMethodWidget, SIGNAL(currentIndexChanged(int)), this,
                        SLOT(onRetrieveMethodChanged(int)));

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::pingPACS()
{
    m_requestWorker->post([&]
        {
            const auto pacsConfiguration =
                this->getLockedInOut< const ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

            ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

            bool success = false;
            try
            {
                seriesEnquirer->initialize(
                    pacsConfiguration->getLocalApplicationTitle(),
                    pacsConfiguration->getPacsHostName(),
                    pacsConfiguration->getPacsApplicationPort(),
                    pacsConfiguration->getPacsApplicationTitle());
                seriesEnquirer->connect();
                success = seriesEnquirer->pingPacs();
            }
            catch (::fwPacsIO::exceptions::Base& _e)
            {
                SLM_ERROR("Can't establish a connection with the PACS: " + std::string(_e.what()));
            }

            if(seriesEnquirer->isConnectedToPacs())
            {
                seriesEnquirer->disconnect();
            }

            // Display a message with the ping result.
            if(m_showDialog)
            {
                if(success)
                {
                    m_slotShowDialog->asyncRun("Ping Pacs", "Ping succeeded!");
                }
                else
                {
                    m_slotShowDialog->asyncRun("Ping Pacs", "Ping failed!");
                }
            }

            if(success)
            {
                const auto notif = this->signal< services::IService::SuccessNotifiedSignalType >(
                    services::IService::s_SUCCESS_NOTIFIED_SIG);
                notif->asyncEmit("Ping succeeded!");
                SLM_INFO("Ping succeeded")
            }
            else
            {
                const auto notif = this->signal< services::IService::FailureNotifiedSignalType >(
                    services::IService::s_FAILURE_NOTIFIED_SIG);
                notif->asyncEmit("Ping failed!");
                SLM_INFO("Ping failed")
            }
        });
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::modifiedNotify(::fwPacsIOdata::PacsConfiguration::sptr _pacsConfiguration)
{
    auto sig = _pacsConfiguration->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onSCUAppEntityTitleChanged()
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setLocalApplicationTitle(m_SCUAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onSCPHostNameChanged()
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setPacsHostName(m_SCPHostNameEdit->text().toStdString());

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onSCPAppEntityTitleChanged()
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setPacsApplicationTitle(m_SCPAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onSCPPortChanged(int value)
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setPacsApplicationPort(static_cast<unsigned short>(value));

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onMoveAppEntityTitleChanged()
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setMoveApplicationTitle(m_moveAppEntityTitleEdit->text().toStdString());

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onMovePortChanged(int _value)
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setMoveApplicationPort(static_cast<unsigned short>(_value));

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::onRetrieveMethodChanged(int _index)
{
    const auto pacsConfiguration = this->getLockedInOut< ::fwPacsIOdata::PacsConfiguration >(s_CONFIG_INOUT);

    pacsConfiguration->setRetrieveMethod(
        (_index ==
         0) ? (::fwPacsIOdata::PacsConfiguration::MOVE_RETRIEVE_METHOD): (::fwPacsIOdata::PacsConfiguration::
                                                                          GET_RETRIEVE_METHOD));

    this->modifiedNotify(pacsConfiguration.get_shared());
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::showDialog(const std::string _title, const std::string _message)
{
    gui::dialog::MessageDialog messageBox;
    messageBox.setTitle(_title);
    messageBox.setMessage(_message);
    messageBox.setIcon(gui::dialog::IMessageDialog::INFO);
    messageBox.addButton(gui::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace ioPacs
