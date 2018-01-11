/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/SSlider.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QString>
#include <QTime>

#include <chrono>

namespace guiQt
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::guiQt::editor::SSlider, ::fwData::Object );

const ::fwCom::Signals::SignalKeyType SSlider::s_VALUE_CHANGED_SIG = "valueChanged";

const ::fwCom::Slots::SlotKeyType SSlider::s_SET_VALUE_SLIDER_SLOT     = "setValue";
const ::fwCom::Slots::SlotKeyType SSlider::s_SET_MIN_VALUE_SLIDER_SLOT = "setMinValue";
const ::fwCom::Slots::SlotKeyType SSlider::s_SET_MAX_VALUE_SLIDER_SLOT = "setMaxValue";

//------------------------------------------------------------------------------

SSlider::SSlider() noexcept :
    m_value(0),
    m_minValue(0),
    m_maxValue(100),
    m_defaultValue(0),
    m_text(""),
    m_isUpdatedOnRelease(false),
    m_hasResetButton(false),
    m_hasEditBox(false),
    m_sliderPressed(false)
{
    newSlot(s_SET_VALUE_SLIDER_SLOT, &SSlider::setValue, this);
    newSlot(s_SET_MIN_VALUE_SLIDER_SLOT, &SSlider::setMinValue, this);
    newSlot(s_SET_MAX_VALUE_SLIDER_SLOT, &SSlider::setMaxValue, this);

    m_sigValueChanged = newSignal<ValueChangedSignalType>(s_VALUE_CHANGED_SIG);
}

//------------------------------------------------------------------------------

SSlider::~SSlider() noexcept
{
}

//------------------------------------------------------------------------------
void SSlider::configuring()
{
    this->initialize();

    // VALUE
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("value");
        if(config)
        {
            m_value = std::stoi(config->getValue());
        }
    }

    // DEFAULT VALUE
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("defaultValue");
        if(config)
        {
            m_defaultValue = std::stoi(config->getValue());
        }
    }

    // RESET BUTTON
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("resetButton");
        if(config)
        {
            m_hasResetButton = (config->getValue() == "true");
        }
    }

    // UPDATE ON RELEASE
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("updateOnRelease");
        if(config)
        {
            m_isUpdatedOnRelease = (config->getValue() == "true");
        }
    }

    // EDIT BOX
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("editBox");
        if(config)
        {
            m_hasEditBox = (config->getValue() == "true");
        }
    }

    // TEXT
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("text");
        if(config)
        {
            m_text = QString(config->getValue().c_str());
        }
    }

    // RANGE
    {
        ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("range");
        if(config)
        {
            ::fwRuntime::ConfigurationElement::sptr minCfg = config->findConfigurationElement("min");
            ::fwRuntime::ConfigurationElement::sptr maxCfg = config->findConfigurationElement("max");
            SLM_ASSERT("Missing min and max configuration", minCfg && maxCfg);

            m_minValue = std::stoi(minCfg->getValue());
            m_maxValue = std::stoi(maxCfg->getValue());
        }
    }
}

//------------------------------------------------------------------------------
void SSlider::starting()
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    m_valueSlider = new QSlider(Qt::Horizontal);
    m_valueSlider->setRange(m_minValue, m_maxValue);
    m_valueSlider->setValue(m_value);

    QMetaObject::Connection isConnected;
    isConnected = QObject::connect(m_valueSlider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    SLM_ASSERT("sliderPressed Signal failed to connect to sliderPressed Slot.", isConnected);

    isConnected = QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    SLM_ASSERT("valueChanged Signal failed to connect to setValue Slot.", isConnected);

    isConnected = QObject::connect(m_valueSlider, SIGNAL(sliderReleased()), this, SLOT(changeValue()));
    SLM_ASSERT("sliderReleased Signal failed to connect to changeValue Slot.", isConnected);

    m_textLabel = new QLabel();
    m_textLabel->setText(m_text);

    if( !m_hasEditBox )
    {
        m_valueLabel = new QLabel();
        m_valueLabel->setText(QString::number(m_value));
    }

    m_minValueLabel = new QLabel();
    m_minValueLabel->setText(QString::number(m_minValue));

    m_maxValueLabel = new QLabel();
    m_maxValueLabel->setText(QString::number(m_maxValue));

    layout->addWidget( m_textLabel );

    if( !m_hasEditBox )
    {
        layout->addWidget( m_valueLabel );
    }
    layout->addWidget( m_minValueLabel );
    layout->addWidget( m_valueSlider );
    layout->addWidget( m_maxValueLabel );

    if( m_hasEditBox )
    {
        m_valueEdit = new QLineEdit("");
        m_valueEdit->setMaximumWidth( 70 );
        m_valueEdit->setInputMask("#0000");

        isConnected = QObject::connect( m_valueEdit, SIGNAL( returnPressed()), this, SLOT(editValue()) );
        SLM_ASSERT("editingFinished Signal failed to connect to onTextChanged Slot.", isConnected);

        layout->addWidget( m_valueEdit );
    }

    if( m_hasResetButton )
    {
        m_resetButton = new QPushButton("R");  // "R" is codename for Reset !

        isConnected = QObject::connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetValue()));
        SLM_ASSERT("clicked Signal failed to connect to resetValue Slot.", isConnected);

        layout->addWidget( m_resetButton );

    }

    qtContainer->setLayout(layout);

    this->resetValue();
}

//------------------------------------------------------------------------------

void SSlider::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));

    this->destroy();
}
//------------------------------------------------------------------------------

void SSlider::updating()
{
}

//------------------------------------------------------------------------------

void SSlider::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------
void SSlider::sliderPressed()
{
    m_sliderPressed = true;
}

//------------------------------------------------------------------------------
void SSlider::resetValue()
{
    setValue( m_defaultValue, true );
}

//------------------------------------------------------------------------------
void SSlider::changeValue()
{
    SLM_ASSERT( "m_valueSlider must not be null",        nullptr != m_valueSlider );
    SLM_ASSERT( "m_sigValueChanged must not be null",    nullptr != m_sigValueChanged );

    int value = m_valueSlider->sliderPosition();
    m_valueSlider->setSliderPosition(value);

    // we use either an edit box or a label to display the current value
    if( !m_hasEditBox )
    {
        SLM_ASSERT("m_valueLabel must not be null", nullptr != m_valueLabel);

        m_valueLabel->setText(QString::number(value));
    }
    else
    {
        SLM_ASSERT("m_valueEdit must not be null", nullptr != m_valueEdit);

        m_valueEdit->setText(QString::number(value));
    }

    // Notify the new position
    m_sigValueChanged->asyncEmit(value);

    m_sliderPressed = false;
}

//------------------------------------------------------------------------------

void SSlider::editValue()
{
    SLM_ASSERT("m_valueEdit must not be null", false );// && nullptr != m_valueEdit);

    QString strValue = m_valueEdit->text();

    setValue( strValue.toInt(), true );
}

//------------------------------------------------------------------------------

void SSlider::setValue( int value, bool _bForced )
{
    SLM_ASSERT("m_valueSlider must not be null", nullptr != m_valueSlider);

    // we use either an edit box or a label to display the current value
    if( !m_hasEditBox )
    {
        SLM_ASSERT("m_valueLabel must not be null", nullptr != m_valueLabel);

        m_valueLabel->setText(QString::number(value));
    }
    else
    {
        SLM_ASSERT("m_valueEdit must not be null", nullptr != m_valueEdit);

        m_valueEdit->setText(QString::number(value));
    }

    if( !m_sliderPressed || !m_isUpdatedOnRelease || _bForced )
    {
        m_value = value;
        m_valueSlider->setValue(value);

        // Notify the new position
        m_sigValueChanged->asyncEmit(value);
    }
}

//------------------------------------------------------------------------------

void SSlider::setMinValue(int value)
{
    SLM_ASSERT("m_valueSlider must not be null", nullptr != m_valueSlider);
    SLM_ASSERT("m_valueSlider must not be null", nullptr != m_valueSlider);

    m_minValue = value;
    m_valueSlider->setMinimum( value );
    m_minValueLabel->setText( QString::number(value));
}

//------------------------------------------------------------------------------

void SSlider::setMaxValue(int value)
{
    SLM_ASSERT("m_valueSlider must not be null", nullptr != m_valueSlider);
    SLM_ASSERT("m_maxValueLabel must not be null", nullptr != m_maxValueLabel);

    m_maxValue = value;
    m_valueSlider->setMaximum( value );
    m_maxValueLabel->setText( QString::number(value));
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace guiQt
