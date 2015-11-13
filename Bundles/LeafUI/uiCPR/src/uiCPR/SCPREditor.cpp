/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>

#include <fwData/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <cpr/functions.hpp>

#include "uiCPR/SCPREditor.hpp"

#include <fwCom/Signal.hxx>


fwServicesRegisterMacro(::gui::editor::IEditor, ::uiCPR::SCPREditor, ::fwData::Object);

namespace uiCPR
{

const double SCPREditor::s_MIN_HEIGHT = 20;
const int MAX_ANGLE                   = 180;

const ::fwCom::Signals::SignalKeyType SCPREditor:: s_HEIGHT_CHANGED_SIG    = "heightChanged";
const ::fwCom::Signals::SignalKeyType SCPREditor:: s_SPACING_CHANGED_SIG   = "spacingChanged";
const ::fwCom::Signals::SignalKeyType SCPREditor:: s_SLIDER_PROGRESSED_SIG = "sliderProgressed";

SCPREditor::SCPREditor() throw() : m_spacing(0.5), m_height(50), m_angle(0)
{
    // Init
    m_sigHeightChanged    = HeightChangedSignalType::New();
    m_sigSpacingChanged   = SpacingChangedSignalType::New();
    m_sigSliderProgressed = SliderProgressedSignalType::New();

    // Register
    ::fwCom::HasSignals::m_signals(s_HEIGHT_CHANGED_SIG, m_sigHeightChanged);
    ::fwCom::HasSignals::m_signals(s_SPACING_CHANGED_SIG,m_sigSpacingChanged);
    ::fwCom::HasSignals::m_signals(s_SLIDER_PROGRESSED_SIG,m_sigSliderProgressed);
}

//------------------------------------------------------------------------------------------------------

SCPREditor::~SCPREditor() throw()
{
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    // Get the Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("Failed to instanciate container", container);

    // Get the source image to set the minimum and maximum of spacing and height.
    ::fwTools::Object::sptr imgObj = ::fwTools::fwID::getObject(m_sourceImageUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_sourceImageUID << "'", imgObj);
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(imgObj);
    OSLM_ASSERT("Failed to retrieve iamge", image);

    m_spacing    = ::cpr::getImageMinSpacing(image);
    m_minSpacing = m_spacing;
    m_maxSpacing = ::cpr::getImageMaxSpacing(image) * 2;

    m_maxHeight = ::cpr::getImageMaxSize(image) / 2.0;

    m_heightSpinBox = new QDoubleSpinBox();
    m_heightSpinBox->setRange(s_MIN_HEIGHT, m_maxHeight);
    m_heightSpinBox->setSingleStep(1);
    m_heightSpinBox->setValue(50);

    m_spacingSpinBox = new QDoubleSpinBox();
    m_spacingSpinBox->setRange(m_minSpacing, m_maxSpacing);
    m_spacingSpinBox->setSingleStep(0.01);
    m_spacingSpinBox->setValue(m_minSpacing);

    m_computeButton = new QPushButton("Compute");
    m_computeButton->setEnabled(false);

    m_rotationSlider = new QSlider(Qt::Horizontal);
    m_rotationSlider->setRange(0, MAX_ANGLE);
    m_rotationSlider->setValue(0);

    m_angleText = new QLineEdit();
    m_angleText->setReadOnly(true);
    m_angleText->setMaximumWidth(80);
    m_angleText->setText(QString::fromStdString(" 0 / 180 "));

    QObject::connect(m_heightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onChangeHeightValue(double)));
    QObject::connect(m_spacingSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onChangeSpacingValue(double)));
    QObject::connect(m_computeButton, SIGNAL(clicked()), this,SLOT(onClickComputeSlotType()));
    QObject::connect(m_rotationSlider, SIGNAL(valueChanged(int)), this, SLOT(onChangeSliderValue(int)));

    QGridLayout* hLayout = new QGridLayout(container);
    hLayout->addWidget(new QLabel("Height:"), 0, 0, 1, 1);
    hLayout->addWidget(m_heightSpinBox, 0, 1, 1, 1);
    hLayout->addWidget(new QLabel("Spacing :"), 0, 2, 1, 1);
    hLayout->addWidget(m_spacingSpinBox, 0, 3, 1, 1);
    hLayout->addWidget(m_computeButton, 0, 4, 1, 1);
    hLayout->addWidget(m_rotationSlider, 1, 0, 1, 3);
    hLayout->addWidget(m_angleText, 1, 3, 1, 1);

    container->setLayout(hLayout);
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_heightSpinBox, SIGNAL(valueChanged (double)), this,SLOT(onChangeHeightValue(double)));
    QObject::disconnect(m_spacingSpinBox, SIGNAL(valueChanged (double)), this,SLOT(onChangeSpacingValue(double)));
    QObject::disconnect(m_rotationSlider, SIGNAL(valueChanged (int)), this,SLOT(onChangeSliderValue(int)));
    QObject::disconnect(m_computeButton, SIGNAL(clicked()), this,SLOT(onClickComputeSlotType()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    std::vector<ConfigurationType> sourceImageConfig = m_configuration->find("sourceImage");
    if (!sourceImageConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", sourceImageConfig.at(0)->hasAttribute("uid"));
        m_sourceImageUID = (sourceImageConfig.at(0)->getAttributeValue("uid"));
    }
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::updating() throw(::fwTools::Failed)
{
    // Send the signals to update the CPR mesh when the view debug is launched
    m_sigSpacingChanged->asyncEmit(m_spacing);
    m_sigHeightChanged->asyncEmit(m_height);
    m_sigSliderProgressed->asyncEmit(m_angle);
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::receiving(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::onChangeSliderValue(int newSliderValue)
{
    m_angle = newSliderValue;
    std::stringstream ss;
    ss << " " << newSliderValue << " / " << MAX_ANGLE << " ";
    m_angleText->setText(QString::fromStdString(ss.str()));
    m_sigSliderProgressed->asyncEmit(newSliderValue);
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::onChangeHeightValue(double newHeight)
{
    m_height = newHeight;

    if(m_height < s_MIN_HEIGHT)
    {
        m_height = s_MIN_HEIGHT;
    }
    else if(m_height > m_maxHeight)
    {
        m_height = m_maxHeight;
    }

    OSLM_DEBUG("Change Height : " << m_height);
    m_sigHeightChanged->asyncEmit(m_height);
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::onChangeSpacingValue(double newSpacing)
{
    m_spacing = newSpacing;

    if(m_spacing < m_minSpacing)
    {
        m_spacing = m_minSpacing;
    }
    else if(m_spacing > m_maxSpacing)
    {
        m_spacing = m_maxSpacing;
    }

    OSLM_DEBUG("Change Spacing : " << m_spacing);
    m_computeButton->setEnabled(true);
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::onClickComputeSlotType()
{
    m_sigSpacingChanged->asyncEmit(m_spacing);
    m_computeButton->setEnabled(false);
}

}//namespace uiSpline

