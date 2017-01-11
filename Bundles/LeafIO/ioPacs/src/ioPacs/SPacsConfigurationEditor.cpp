/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SPacsConfigurationEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwPacsIO/exceptions/Base.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwServices/macros.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <boost/foreach.hpp>

namespace ioPacs
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::ioPacs::SPacsConfigurationEditor,
                         ::fwPacsIO::data::PacsConfiguration );

//------------------------------------------------------------------------------

SPacsConfigurationEditor::SPacsConfigurationEditor() throw()
{
}
//------------------------------------------------------------------------------

SPacsConfigurationEditor::~SPacsConfigurationEditor() throw()
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::info(std::ostream& _sstream )
{
    _sstream << "SPacsConfigurationEditor::info";
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    SLM_ASSERT("Pacs configuration object should not be null.", pacsConfiguration);


    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();
    QGridLayout* gridLayout  = new QGridLayout();
    container->setLayout(gridLayout);

    // Local application title
    m_localApplicationTitleWidget = new QLineEdit();
    m_localApplicationTitleWidget->setText(pacsConfiguration->getLocalApplicationTitle().c_str());
    gridLayout->addWidget(new QLabel("Local application title:"),0, 0);
    gridLayout->addWidget(m_localApplicationTitleWidget,0, 1);

    // Pacs host name
    m_pacsHostNameWidget = new QLineEdit();
    m_pacsHostNameWidget->setText(pacsConfiguration->getPacsHostName().c_str());
    gridLayout->addWidget(new QLabel("Pacs host name:"),2, 0);
    gridLayout->addWidget(m_pacsHostNameWidget,2, 1);

    // Pacs application title
    m_pacsApplicationTitleWidget = new QLineEdit();
    m_pacsApplicationTitleWidget->setText(pacsConfiguration->getPacsApplicationTitle().c_str());
    gridLayout->addWidget(new QLabel("Pacs application title:"),3, 0);
    gridLayout->addWidget(m_pacsApplicationTitleWidget,3, 1);

    // Pacs application port
    m_pacsApplicationPortWidget = new QSpinBox();
    m_pacsApplicationPortWidget->setRange(0,65535);
    m_pacsApplicationPortWidget->setValue(pacsConfiguration->getPacsApplicationPort());
    gridLayout->addWidget(new QLabel("Pacs application port:"),4, 0);
    gridLayout->addWidget(m_pacsApplicationPortWidget,4, 1);

    // Move application title
    m_moveApplicationTitleWidget = new QLineEdit();
    m_moveApplicationTitleWidget->setText(pacsConfiguration->getMoveApplicationTitle().c_str());
    gridLayout->addWidget(new QLabel("Move application title:"),5, 0);
    gridLayout->addWidget(m_moveApplicationTitleWidget,5, 1);

    // Move application port
    m_moveApplicationPortWidget = new QSpinBox();
    m_moveApplicationPortWidget->setRange(0,65535);
    m_moveApplicationPortWidget->setValue(pacsConfiguration->getMoveApplicationPort());
    gridLayout->addWidget(new QLabel("Move application port:"),6, 0);
    gridLayout->addWidget(m_moveApplicationPortWidget,6, 1);

    // Retrieve method
    m_retrieveMethodWidget = new QComboBox();
    m_retrieveMethodWidget->addItem("Move");
    m_retrieveMethodWidget->addItem("Get");
    m_retrieveMethodWidget->setCurrentIndex(
        (pacsConfiguration->getRetrieveMethod() == ::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD) ? 0 : 1);
    gridLayout->addWidget(new QLabel("Retrieve method:"),7, 0);
    gridLayout->addWidget(m_retrieveMethodWidget,7, 1);

    // Test button
    m_pingPacsButtonWidget = new QPushButton("Ping Pacs");
    gridLayout->addWidget(m_pingPacsButtonWidget, 8,0,1,2);

    // Connect the signals
    QObject::connect(m_pingPacsButtonWidget, SIGNAL(clicked()), this, SLOT(pingPacs()));
    QObject::connect(m_localApplicationTitleWidget, SIGNAL(editingFinished()), this, SLOT(
                         localApplicationTitleChanged()));
    QObject::connect(m_pacsHostNameWidget, SIGNAL(editingFinished()), this, SLOT(pacsHostNameChanged()));
    QObject::connect(m_pacsApplicationTitleWidget, SIGNAL(editingFinished()), this,
                     SLOT(pacsApplicationTitleChanged()));
    QObject::connect(m_pacsApplicationPortWidget, SIGNAL(valueChanged(int)), this,
                     SLOT(pacsApplicationPortChanged(int)));
    QObject::connect(m_moveApplicationTitleWidget, SIGNAL(editingFinished()), this,
                     SLOT(moveApplicationTitleChanged()));
    QObject::connect(m_moveApplicationPortWidget, SIGNAL(valueChanged(int)), this,
                     SLOT(moveApplicationPortChanged(int)));
    QObject::connect(m_retrieveMethodWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(retrieveMethodChanged(int)));

}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Disconnect the signals
    QObject::disconnect(m_pingPacsButtonWidget, SIGNAL(clicked()), this, SLOT(pingPacs()));
    QObject::disconnect(m_localApplicationTitleWidget, SIGNAL(editingFinished()), this,
                        SLOT(localApplicationTitleChanged()));
    QObject::disconnect(m_pacsHostNameWidget, SIGNAL(editingFinished()), this, SLOT(pacsHostNameChanged()));
    QObject::disconnect(m_pacsApplicationTitleWidget, SIGNAL(editingFinished()), this,
                        SLOT(pacsApplicationTitleChanged()));
    QObject::disconnect(m_pacsApplicationPortWidget, SIGNAL(valueChanged(int)), this,
                        SLOT(pacsApplicationPortChanged(int)));
    QObject::disconnect(m_moveApplicationTitleWidget, SIGNAL(editingFinished()), this,
                        SLOT(moveApplicationTitleChanged()));
    QObject::disconnect(m_moveApplicationPortWidget, SIGNAL(valueChanged(int)), this,
                        SLOT(moveApplicationPortChanged(int)));
    QObject::disconnect(m_retrieveMethodWidget, SIGNAL(currentIndexChanged(int)), this,
                        SLOT(retrieveMethodChanged(int)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::pingPacs()
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration
        = this->getObject< ::fwPacsIO::data::PacsConfiguration >();

    ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    bool success = false;
    try
    {
        seriesEnquirer->initialize(
            pacsConfiguration->getLocalApplicationTitle(),
            pacsConfiguration->getPacsHostName(),
            pacsConfiguration->getPacsApplicationPort(),
            pacsConfiguration->getPacsApplicationTitle(),
            pacsConfiguration->getMoveApplicationTitle());
        seriesEnquirer->connect();
        success = seriesEnquirer->pingPacs();
        seriesEnquirer->disconnect();
    }
    catch (::fwPacsIO::exceptions::Base& exception)
    {
        SLM_TRACE(exception.what());
        success = false;
    }

    // Display a message with the ping result.
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Ping Pacs");
    if(success)
    {
        messageBox.setMessage( "Ping succeed !" );
    }
    else
    {
        messageBox.setMessage( "Ping failed !" );
    }
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();

}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::modifiedNotify(::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration)
{
    auto sig = pacsConfiguration->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::localApplicationTitleChanged()
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setLocalApplicationTitle(m_localApplicationTitleWidget->text().toStdString());

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::pacsHostNameChanged()
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setPacsHostName(m_pacsHostNameWidget->text().toStdString());

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::pacsApplicationTitleChanged()
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setPacsApplicationTitle(m_pacsApplicationTitleWidget->text().toStdString());

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::pacsApplicationPortChanged(int value)
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setPacsApplicationPort(static_cast<unsigned short>(value));

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::moveApplicationTitleChanged()
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setMoveApplicationTitle(m_moveApplicationTitleWidget->text().toStdString());

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------


void SPacsConfigurationEditor::moveApplicationPortChanged(int value)
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setMoveApplicationPort(static_cast<unsigned short>(value));

    modifiedNotify(pacsConfiguration);
}

//------------------------------------------------------------------------------

void SPacsConfigurationEditor::retrieveMethodChanged(int index)
{
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();
    pacsConfiguration->setRetrieveMethod(
        (index==
         0) ? (::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD): (::fwPacsIO::data::PacsConfiguration::
                                                                            GET_RETRIEVE_METHOD));

    modifiedNotify(pacsConfiguration);
}

} // namespace ioPacs
