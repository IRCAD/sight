/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiParameters/editor/InputParameters.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>
#include <fwTools/Object.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <QWidget>
#include <QFormLayout>
#include <QPushButton>
#include <QString>
#include <QCheckBox>
#include <QSpinBox>

namespace uiParameters
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiParameters::editor::InputParameters, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType TRIGGERED_SIG      = "triggered";
static const ::fwCom::Signals::SignalKeyType INTEGERCHANGED_SIG = "integerChanged";
static const ::fwCom::Signals::SignalKeyType BOOLEANCHANGED_SIG = "booleanChanged";

//-----------------------------------------------------------------------------

InputParameters::InputParameters() throw () : m_sigTriggered(TrigerredSignalType::New()),
                                              m_sigIntegerChanged(IntegerChangedSignalType::New()),
                                              m_sigBooleanChanged(BooleanChangedSignalType::New())
{
    ::fwCom::HasSignals::m_signals(TRIGGERED_SIG, m_sigTriggered);
    ::fwCom::HasSignals::m_signals(INTEGERCHANGED_SIG, m_sigIntegerChanged);
    ::fwCom::HasSignals::m_signals(BOOLEANCHANGED_SIG, m_sigBooleanChanged);

}

//-----------------------------------------------------------------------------

InputParameters::~InputParameters() throw ()
{
}

//-----------------------------------------------------------------------------

void InputParameters::starting() throw (::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QFormLayout* layout = new QFormLayout(container);
    container->setLayout(layout);

    std::vector< ConfigurationType > parameters = m_configuration->find("parameters");
    SLM_ASSERT("There must be at least on parameters tag", parameters.size() == 1);
    std::vector< ConfigurationType > params = parameters[0]->find("param");


    for(ConfigurationType param : params)
    {
        std::string name = param->getExistingAttributeValue("name");
        std::string key  = param->getExistingAttributeValue("key");
        std::string type = param->getExistingAttributeValue("type");

        if(type=="::fwData::Integer")
        {
            QSpinBox* spinbox = new QSpinBox();

            if(param->hasAttribute("min"))
            {
                std::string min = param->getExistingAttributeValue("min");
                spinbox->setMinimum(atoi(min.c_str()));
            }
            if(param->hasAttribute("max"))
            {
                std::string max = param->getExistingAttributeValue("max");
                spinbox->setMaximum(atoi(max.c_str()));
            }
            if(param->hasAttribute("defaultValue"))
            {
                std::string defaultValue = param->getExistingAttributeValue("defaultValue");
                int value                = atoi(defaultValue.c_str());

                spinbox->setValue(value);

                m_sigIntegerChanged->asyncEmit(IntegerParameter(key, value));
            }

            spinbox->setProperty("key", QString(key.c_str()));
            layout->addRow(QString(name.c_str()), spinbox);

            QObject::connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));
        }
        else if(type=="::fwData::Boolean")
        {
            QCheckBox* checkbox = new QCheckBox();
            checkbox->setTristate(false);

            if(param->hasAttribute("defaultValue"))
            {
                std::string defaultValue = param->getExistingAttributeValue("defaultValue");
                if(defaultValue == "true")
                {
                    checkbox->setCheckState(Qt::Checked);

                    m_sigBooleanChanged->asyncEmit(BooleanParameter(key, true));
                }
            }

            checkbox->setProperty("key", QString(key.c_str()));
            layout->addRow(QString(name.c_str()), checkbox);

            QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeBoolean(int)));
        }
    }

    QPushButton* button = new QPushButton(tr("Ok"));
    layout->addRow(button);

    QObject::connect(button, SIGNAL(clicked()), this, SLOT(onClick()));

}

//-----------------------------------------------------------------------------

void InputParameters::stopping() throw (::fwTools::Failed)
{
    this->getContainer()->clean();
    this->destroy();
}

//-----------------------------------------------------------------------------

void InputParameters::onChangeInteger(int value)
{
    QObject* sender = this->sender();
    QString key     = sender->property("key").toString();
    m_sigIntegerChanged->asyncEmit(IntegerParameter(key.toStdString(), value));
}

//-----------------------------------------------------------------------------

void InputParameters::onChangeBoolean(int value)
{
    QObject* sender = this->sender();
    QString key     = sender->property("key").toString();
    bool checked    = value == Qt::Checked;

    m_sigBooleanChanged->asyncEmit(BooleanParameter(key.toStdString(), checked));

}
//-----------------------------------------------------------------------------

void InputParameters::onClick()
{
    m_sigTriggered->asyncEmit();
}

//-----------------------------------------------------------------------------

void InputParameters::configuring() throw (::fwTools::Failed)
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void InputParameters::info(std::ostream &_sstream)
{
}

//-----------------------------------------------------------------------------

void InputParameters::updating() throw (::fwTools::Failed)
{
    m_sigTriggered->asyncEmit();
}

//-----------------------------------------------------------------------------

}   //namespace editor
}   //namespace uiParameters
