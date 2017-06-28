/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNavigation/SNavigationEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTools/fwID.hpp>

#include <boost/lexical_cast.hpp>

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include <functional>

fwServicesRegisterMacro(::gui::editor::IEditor, ::uiNavigation::SNavigationEditor, ::fwData::Object);

namespace uiNavigation
{

const ::fwCom::Signals::SignalKeyType SNavigationEditor::s_SLIDER_PROGRESSED_SIG = "sliderProgressed";

//-------------------------------------------------------------------------------------------------------------

SNavigationEditor::SNavigationEditor() throw() :
    m_sliderPosition(0),
    m_sliderLength(100),
    m_timerDuration(50),
    m_iterationsNumber(100)
{
    // Init
    m_sigSliderProgressed = SliderProgressedSignalType::New();

    // Register
    ::fwCom::HasSignals::m_signals( s_SLIDER_PROGRESSED_SIG, m_sigSliderProgressed);
}

//-------------------------------------------------------------------------------------------------------------

SNavigationEditor::~SNavigationEditor() throw ()
{
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    // Buttons
    m_playButton = new QPushButton(QObject::tr("Play"));

    m_pauseButton = new QPushButton(QObject::tr("Pause"));
    m_pauseButton->setEnabled(false);

    m_resetButton = new QPushButton(QObject::tr("Reset"));
    m_resetButton->setEnabled(false);

    // Create the slider
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setRange(0, m_sliderLength);
    m_slider->setValue(0);

    m_progressText = new QLineEdit();
    m_progressText->setReadOnly(true);
    m_progressText->setMaximumWidth(80);

    std::stringstream ss;
    ss << " 0 / " << m_sliderLength << " ";
    m_progressText->setText(QString::fromStdString(ss.str()));

    QObject::connect(m_playButton, SIGNAL(clicked()), this, SLOT(onClickPlaySlotType()));
    QObject::connect(m_pauseButton, SIGNAL(clicked()), this, SLOT(onClickPauseSlotType()));
    QObject::connect(m_resetButton, SIGNAL(clicked()), this, SLOT(onClickResetSlotType()));
    QObject::connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeValue(int)));

    // Create the layout
    QGridLayout* hLayout = new QGridLayout();
    hLayout->addWidget(m_playButton, 0, 1, 1, 1);
    hLayout->addWidget(m_pauseButton, 0, 2, 1, 1);
    hLayout->addWidget(m_resetButton, 0, 3, 1, 1);
    hLayout->addWidget(m_slider, 1, 0, 1, 3);
    hLayout->addWidget(m_progressText, 1, 3, 1, 1);
    qtContainer->setLayout(hLayout);

    // Timer
    SLM_ASSERT("No valid worker for fly mode", m_associatedWorker);
    ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(m_timerDuration);
    m_timer                                      = m_associatedWorker->createTimer();
    m_timer->setFunction(std::bind(&SNavigationEditor::callBackTimer, this));
    m_timer->setDuration(duration);
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_playButton, SIGNAL(clicked()), this, SLOT(onClickPlaySlotType()));
    QObject::disconnect(m_pauseButton, SIGNAL(clicked()), this, SLOT(onClickPauseSlotType()));
    QObject::disconnect(m_resetButton, SIGNAL(clicked()), this, SLOT(onClickResetSlotType()));
    QObject::disconnect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeValue(int)));

    this->destroy();
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin();
    for(; iter != this->m_configuration->end(); ++iter)
    {
        // Get slider length
        if((*iter)->getName() == "sliderLength")
        {
            SLM_ASSERT("XML element <sliderLength> must include attribute 'value'", (*iter)->hasAttribute("value"));
            const std::string value = (*iter)->getExistingAttributeValue("value");
            m_sliderLength = boost::lexical_cast<int>(value);
            SLM_ASSERT("Attribute value of XML element <iterationsNumber> must be positive.", m_sliderLength >= 0);
        }

        // Get timer duration
        if((*iter)->getName() == "timerDuration")
        {
            SLM_ASSERT("XML element <timerDuration> must include attribute 'value'", (*iter)->hasAttribute("value"));
            const std::string value = (*iter)->getExistingAttributeValue("value");
            m_timerDuration = boost::lexical_cast<int>(value);
            SLM_ASSERT("Attribute value of XML element <duration> must be positive.", m_timerDuration >= 0);
        }

        // Get iterations number
        if((*iter)->getName() == "iterationsNumber")
        {
            SLM_ASSERT("XML element <iterationsNumber> must include attribute 'value'", (*iter)->hasAttribute("value"));
            const std::string value = (*iter)->getExistingAttributeValue("value");
            m_iterationsNumber = boost::lexical_cast<double>(value);
            SLM_ASSERT("'<iterationsNumber>' value must be positive.", m_iterationsNumber >= 0);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::updating() throw(::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::onChangeValue(int newValue)
{
    m_resetButton->setEnabled(false);
    m_playButton->setEnabled(true);
    m_sliderPosition = newValue;

    std::stringstream ss;
    ss << " " << newValue << " / " << m_sliderLength << " ";

    m_progressText->setText(QString::fromStdString(ss.str()));

    m_sigSliderProgressed->asyncEmit(m_sliderPosition / (m_sliderLength * 1.0));

    if(m_sliderPosition >= m_sliderLength)
    {
        m_playButton->setEnabled(false);
        m_resetButton->setEnabled(true);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::onClickPlaySlotType()
{
    if(!m_timer->isRunning() && m_sliderPosition < m_sliderLength)
    {
        m_slider->setEnabled(false);
        QObject::disconnect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeValue(int)));
        m_timer->start();
        m_playButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_resetButton->setEnabled(false);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::onClickPauseSlotType()
{
    if(m_timer->isRunning())
    {
        m_timer->stop();
        m_slider->setEnabled(true);
        QObject::connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeValue(int)));
        m_pauseButton->setEnabled(false);
        m_playButton->setEnabled(true);
        m_resetButton->setEnabled(true);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::onClickResetSlotType()
{
    if(!m_timer->isRunning())
    {
        m_sigSliderProgressed->asyncEmit(0);

        m_sliderPosition = 0;
        m_slider->setValue(m_sliderPosition);

        m_playButton->setEnabled(true);
        m_resetButton->setEnabled(false);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SNavigationEditor::callBackTimer()
{
    SLM_TRACE_FUNC();

    m_sliderPosition += m_sliderLength / (m_iterationsNumber * 1.0);

    if(m_sliderPosition >= m_sliderLength)
    {
        m_timer->stop();
        m_sliderPosition = m_sliderLength;
        m_slider->setEnabled(true);

        QObject::connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeValue(int)));

        m_resetButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
        m_playButton->setEnabled(false);
    }

    m_sigSliderProgressed->asyncEmit(m_sliderPosition / (m_sliderLength * 1.0));
    m_slider->setValue(static_cast<int>(m_sliderPosition));

    std::stringstream ss;
    ss << " " << m_sliderPosition << " / " << m_sliderLength << " ";

    m_progressText->setText(QString::fromStdString(ss.str()));
}

}   // namespace uiNavigation

