/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCPR/SCPREditor.hpp"

#include <fwData/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/op/Get.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <cpr/functions.hpp>

#include <fwCom/Signal.hxx>

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>

fwServicesRegisterMacro(::gui::editor::IEditor, ::uiCPR::SCPREditor, ::fwData::Image);

namespace uiCPR
{

static const std::string s_IMAGE_KEY = "image";

const double SCPREditor::s_MIN_HEIGHT = 20.;
const int MAX_ANGLE                   = 180;

const ::fwCom::Signals::SignalKeyType SCPREditor:: s_HEIGHT_CHANGED_SIG    = "heightChanged";
const ::fwCom::Signals::SignalKeyType SCPREditor:: s_SPACING_CHANGED_SIG   = "spacingChanged";
const ::fwCom::Signals::SignalKeyType SCPREditor:: s_SLIDER_PROGRESSED_SIG = "sliderProgressed";

SCPREditor::SCPREditor() throw() :
    m_angle(0.),
    m_spacing(0.5),
    m_minSpacing(0.5),
    m_maxSpacing(0.5),
    m_height(50.),
    m_maxHeight(50.)
{
    m_sigHeightChanged    = newSignal<HeightChangedSignalType>(s_HEIGHT_CHANGED_SIG);
    m_sigSpacingChanged   = newSignal<SpacingChangedSignalType>(s_SPACING_CHANGED_SIG);
    m_sigSliderProgressed = newSignal<SliderProgressedSignalType>(s_SLIDER_PROGRESSED_SIG);
}

//------------------------------------------------------------------------------------------------------

SCPREditor::~SCPREditor() throw()
{
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::starting() throw(::fwTools::Failed)
{
    this->create();

    // Get the Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("Failed to instantiate container", container);

    // Get the source image to set the minimum and maximum of spacing and height.
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT( s_IMAGE_KEY + " doesn't exist or is not an image", image);

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
    QObject::connect(m_computeButton, SIGNAL(clicked()), this, SLOT(onClickComputeSlotType()));
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
    QObject::disconnect(m_heightSpinBox, SIGNAL(valueChanged (double)), this, SLOT(onChangeHeightValue(double)));
    QObject::disconnect(m_spacingSpinBox, SIGNAL(valueChanged (double)), this, SLOT(onChangeSpacingValue(double)));
    QObject::disconnect(m_rotationSlider, SIGNAL(valueChanged (int)), this, SLOT(onChangeSliderValue(int)));
    QObject::disconnect(m_computeButton, SIGNAL(clicked()), this, SLOT(onClickComputeSlotType()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------------------------------

void SCPREditor::configuring() throw(fwTools::Failed)
{
    this->initialize();
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

