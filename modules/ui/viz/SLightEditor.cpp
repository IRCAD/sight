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

#include "SLightEditor.hpp"

#include "modules/ui/viz/helper/Utils.hpp"

#include <core/com/Slots.hxx>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Scene.hpp>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

#include <ui/qt/container/QtContainer.hpp>

namespace sight::modules::ui::viz
{

using sight::viz::scene3d::ILight;
using sight::viz::scene3d::Layer;

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::modules::ui::viz::SLightEditor,
                         ::sight::data::Composite )

//------------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_EDIT_LIGHT_SLOT = "editLight";

//------------------------------------------------------------------------------

SLightEditor::SLightEditor() noexcept
{
    newSlot(s_EDIT_LIGHT_SLOT, &SLightEditor::editLight, this);
}

//------------------------------------------------------------------------------

SLightEditor::~SLightEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SLightEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SLightEditor::starting()
{
    this->create();

    const auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer());

    m_lightNameLabel = new QLabel("No light selected");
    m_lightNameLabel->setAlignment(::Qt::AlignHCenter);

    m_lightTypeBox = new QComboBox();
    m_lightTypeBox->addItems(QStringList() <<
                             ILight::s_POINT_LIGHT.c_str() <<
                             ILight::s_DIRECTIONAL_LIGHT.c_str());
    m_lightTypeBox->setEnabled(false);

    m_visualFeedback = new QPushButton("Feedback");
    m_visualFeedback->setCheckable(true);
    m_visualFeedback->setEnabled(false);

    m_diffuseColorBtn = new QPushButton("Diffuse color");
    m_diffuseColorBtn->setEnabled(false);

    m_specularColorBtn = new QPushButton("Specular color");
    m_specularColorBtn->setEnabled(false);

    m_thetaSlider = new QSlider(::Qt::Horizontal);
    m_thetaSlider->setMinimum(0);
    m_thetaSlider->setMaximum(ILight::s_OFFSET_RANGE);
    m_thetaSlider->setEnabled(false);

    m_phiSlider = new QSlider(::Qt::Horizontal);
    m_phiSlider->setMinimum(0);
    m_phiSlider->setMaximum(ILight::s_OFFSET_RANGE);
    m_phiSlider->setEnabled(false);

    m_xTranslation = new QSlider(::Qt::Horizontal);
    m_xTranslation->setMinimum(-2000);
    m_xTranslation->setMaximum(2000);
    m_xTranslation->setEnabled(false);

    m_yTranslation = new QSlider(::Qt::Horizontal);
    m_yTranslation->setMinimum(-2000);
    m_yTranslation->setMaximum(2000);
    m_yTranslation->setEnabled(false);

    m_zTranslation = new QSlider(::Qt::Horizontal);
    m_zTranslation->setMinimum(-2000);
    m_zTranslation->setMaximum(2000);
    m_zTranslation->setEnabled(false);

    m_xLabel = new QLineEdit("X: 0");
    m_xLabel->setReadOnly(true);
    m_xLabel->setMaximumWidth(70);
    m_yLabel = new QLineEdit("Y: 0");
    m_yLabel->setReadOnly(true);
    m_yLabel->setMaximumWidth(70);
    m_zLabel = new QLineEdit("Z: 0");
    m_zLabel->setReadOnly(true);
    m_zLabel->setMaximumWidth(70);

    m_xReset = new QPushButton("Reset");
    m_xReset->setEnabled(false);
    m_yReset = new QPushButton("Reset");
    m_yReset->setEnabled(false);
    m_zReset = new QPushButton("Reset");
    m_zReset->setEnabled(false);

    // Name of the selected light and its type
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_lightNameLabel);

    QHBoxLayout* const typeLayout = new QHBoxLayout();
    typeLayout->addWidget(m_lightTypeBox);
    typeLayout->addWidget(m_visualFeedback);
    layout->addLayout(typeLayout);

    // Diffuse and specular colors
    QHBoxLayout* const colorLayout = new QHBoxLayout();
    colorLayout->addWidget(m_diffuseColorBtn);
    colorLayout->addWidget(m_specularColorBtn);
    layout->addLayout(colorLayout);

    // Theta offset
    QHBoxLayout* const thetaLayout = new QHBoxLayout();
    thetaLayout->addWidget(new QLabel("Theta offset :"));
    thetaLayout->addWidget(m_thetaSlider);
    layout->addLayout(thetaLayout);

    // Phi offset
    QHBoxLayout* const phiLayout = new QHBoxLayout();
    phiLayout->addWidget(new QLabel("Phi offset :"));
    phiLayout->addWidget(m_phiSlider);
    layout->addLayout(phiLayout);

    // Translations;
    QHBoxLayout* const xTransformationLayout = new QHBoxLayout();
    xTransformationLayout->addWidget(m_xLabel, 0);
    xTransformationLayout->addWidget(m_xTranslation, 1);
    xTransformationLayout->addWidget(m_xReset, 0);
    layout->addLayout(xTransformationLayout);

    QHBoxLayout* const yTransformationLayout = new QHBoxLayout();
    yTransformationLayout->addWidget(m_yLabel, 0);
    yTransformationLayout->addWidget(m_yTranslation, 1);
    yTransformationLayout->addWidget(m_yReset, 0);
    layout->addLayout(yTransformationLayout);

    QHBoxLayout* const zTransformationLayout = new QHBoxLayout();
    zTransformationLayout->addWidget(m_zLabel, 0);
    zTransformationLayout->addWidget(m_zTranslation, 1);
    zTransformationLayout->addWidget(m_zReset, 0);
    layout->addLayout(zTransformationLayout);

    qtContainer->setLayout(layout);

    QObject::connect(m_diffuseColorBtn, &QPushButton::clicked, this, &SLightEditor::onEditDiffuseColor);
    QObject::connect(m_specularColorBtn, &QPushButton::clicked, this, &SLightEditor::onEditSpecularColor);

    QObject::connect(m_thetaSlider, &QSlider::valueChanged, this, &SLightEditor::onEditThetaOffset);
    QObject::connect(m_phiSlider, &QSlider::valueChanged, this, &SLightEditor::onEditPhiOffset);

    QObject::connect(m_lightTypeBox, &QComboBox::currentTextChanged, this, &SLightEditor::onEditType);

    QObject::connect(m_visualFeedback, &QPushButton::clicked, this, &SLightEditor::onToggleFeedback);

    QObject::connect(m_xTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditXTranslation);
    QObject::connect(m_yTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditYTranslation);
    QObject::connect(m_zTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditZTranslation);

    QObject::connect(m_xReset, &QPushButton::clicked, this, &SLightEditor::onResetXTranslation);
    QObject::connect(m_yReset, &QPushButton::clicked, this, &SLightEditor::onResetYTranslation);
    QObject::connect(m_zReset, &QPushButton::clicked, this, &SLightEditor::onResetZTranslation);
}

//------------------------------------------------------------------------------

void SLightEditor::updating()
{
}

//------------------------------------------------------------------------------

void SLightEditor::stopping()
{
    QObject::disconnect(m_diffuseColorBtn, &QPushButton::clicked, this, &SLightEditor::onEditDiffuseColor);
    QObject::disconnect(m_specularColorBtn, &QPushButton::clicked, this, &SLightEditor::onEditSpecularColor);

    QObject::disconnect(m_thetaSlider, &QSlider::valueChanged, this, &SLightEditor::onEditThetaOffset);
    QObject::disconnect(m_phiSlider, &QSlider::valueChanged, this, &SLightEditor::onEditPhiOffset);

    QObject::disconnect(m_lightTypeBox, &QComboBox::currentTextChanged, this, &SLightEditor::onEditType);

    QObject::disconnect(m_visualFeedback, &QPushButton::clicked, this, &SLightEditor::onToggleFeedback);

    QObject::disconnect(m_xTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditXTranslation);
    QObject::disconnect(m_yTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditYTranslation);
    QObject::disconnect(m_zTranslation, &QSlider::valueChanged, this, &SLightEditor::onEditZTranslation);

    QObject::disconnect(m_xReset, &QPushButton::clicked, this, &SLightEditor::onResetXTranslation);
    QObject::disconnect(m_yReset, &QPushButton::clicked, this, &SLightEditor::onResetYTranslation);
    QObject::disconnect(m_zReset, &QPushButton::clicked, this, &SLightEditor::onResetZTranslation);

    this->destroy();
}

//------------------------------------------------------------------------------

void SLightEditor::onEditDiffuseColor(bool)
{
    ::Ogre::ColourValue newDiffuseColor = this->editColor(m_currentLight->getDiffuseColor(),
                                                          "Light diffuse color");

    m_currentLight->setDiffuseColor(newDiffuseColor);
}

//------------------------------------------------------------------------------

void SLightEditor::onEditSpecularColor(bool)
{
    ::Ogre::ColourValue newSpecularColor = this->editColor(m_currentLight->getSpecularColor(),
                                                           "Light specular color");

    m_currentLight->setSpecularColor(newSpecularColor);
}

//------------------------------------------------------------------------------

void SLightEditor::onEditThetaOffset(int _value)
{
    m_currentLight->setThetaOffset(static_cast<float>(_value - ILight::s_OFFSET_RANGE / 2));
}

//------------------------------------------------------------------------------

void SLightEditor::onEditPhiOffset(int _value)
{
    m_currentLight->setPhiOffset(static_cast<float>(_value - ILight::s_OFFSET_RANGE / 2));
}

//------------------------------------------------------------------------------

void SLightEditor::onEditType(const QString& _type)
{
    if(_type == ILight::s_POINT_LIGHT.c_str())
    {
        m_currentLight->setType(::Ogre::Light::LT_POINT);
        m_thetaSlider->setEnabled(false);
        m_phiSlider->setEnabled(false);
        if(m_currentLight->getName().find(Layer::s_DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_xTranslation->setEnabled(true);
            m_yTranslation->setEnabled(true);
            m_zTranslation->setEnabled(true);
            m_xReset->setEnabled(true);
            m_yReset->setEnabled(true);
            m_zReset->setEnabled(true);
        }
    }
    else if(_type == ILight::s_DIRECTIONAL_LIGHT.c_str())
    {
        m_currentLight->setType(::Ogre::Light::LT_DIRECTIONAL);
        if(m_currentLight->getName().find(Layer::s_DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_thetaSlider->setEnabled(true);
            m_phiSlider->setEnabled(true);
        }
        m_xTranslation->setEnabled(false);
        m_yTranslation->setEnabled(false);
        m_zTranslation->setEnabled(false);
        m_xReset->setEnabled(false);
        m_yReset->setEnabled(false);
        m_zReset->setEnabled(false);
    }
    else
    {
        SLM_ASSERT("Unknow type for light", false);
    }
    m_currentLight->update();
}

//------------------------------------------------------------------------------

void SLightEditor::onToggleFeedback(bool _enable)
{
    m_currentLight->enableVisualFeedback(_enable);
    m_currentLight->getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void SLightEditor::onEditXTranslation(int _value)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(static_cast< ::Ogre::Real >(_value), currentPos[1], currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_xLabel->setText(QString("X: %1").arg(_value));
}

//------------------------------------------------------------------------------

void SLightEditor::onEditYTranslation(int _value)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(currentPos[0], static_cast< ::Ogre::Real >(_value), currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_yLabel->setText(QString("Y: %1").arg(_value));
}

//------------------------------------------------------------------------------

void SLightEditor::onEditZTranslation(int _value)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(currentPos[0], currentPos[1], static_cast< ::Ogre::Real >(_value)));
    m_currentLight->getRenderService()->requestRender();

    m_zLabel->setText(QString("Z: %1").arg(_value));
}

//------------------------------------------------------------------------------

void SLightEditor::onResetXTranslation(bool)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(0.f, currentPos[1], currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_xLabel->setText("X: 0");
    m_xTranslation->setValue(0);
}

//------------------------------------------------------------------------------

void SLightEditor::onResetYTranslation(bool)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(currentPos[0], 0.f, currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_yLabel->setText("Y: 0");
    m_yTranslation->setValue(0);
}

//------------------------------------------------------------------------------

void SLightEditor::onResetZTranslation(bool)
{
    ::Ogre::Node* const lightNode = this->getLightNode();
    const ::Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(::Ogre::Vector3(currentPos[0], currentPos[1], 0.f));
    m_currentLight->getRenderService()->requestRender();

    m_zLabel->setText("Z: 0");
    m_zTranslation->setValue(0);
}

//------------------------------------------------------------------------------

::Ogre::Node* SLightEditor::getLightNode() const
{
    ::Ogre::SceneNode* const root = m_currentLight->getLayer()->getSceneManager()->getRootSceneNode();
    ::Ogre::Node* const lightNode =
        sight::viz::scene3d::helper::Scene::getNodeById(m_currentLight->getTransformId(), root);
    return lightNode;
}

//------------------------------------------------------------------------------

void SLightEditor::editLight(ILight::sptr _lightAdaptor)
{
    m_currentLight = _lightAdaptor;
    if(_lightAdaptor)
    {
        SLM_ASSERT("The selected light adaptor doesn't exist.", _lightAdaptor);

        m_lightNameLabel->setText(m_currentLight->getName().c_str());
        m_lightTypeBox->setCurrentIndex(static_cast<int>(m_currentLight->getType()));

        m_diffuseColorBtn->setEnabled(true);
        m_specularColorBtn->setEnabled(true);
        m_lightTypeBox->setEnabled(true);

        if(m_currentLight->getName().find(Layer::s_DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_visualFeedback->setEnabled(true);
            if(m_currentLight->getType() == ::Ogre::Light::LT_DIRECTIONAL)
            {
                m_thetaSlider->setEnabled(true);
                m_phiSlider->setEnabled(true);
            }
            else if(m_currentLight->getType() == ::Ogre::Light::LT_POINT)
            {
                m_xTranslation->setEnabled(true);
                m_yTranslation->setEnabled(true);
                m_zTranslation->setEnabled(true);
                m_xReset->setEnabled(true);
                m_yReset->setEnabled(true);
                m_zReset->setEnabled(true);
            }
            else
            {
                SLM_ASSERT("Unknow type for light", false);
            }
        }
        else
        {
            m_visualFeedback->setEnabled(false);
            m_thetaSlider->setEnabled(false);
            m_phiSlider->setEnabled(false);
            m_xTranslation->setEnabled(false);
            m_yTranslation->setEnabled(false);
            m_zTranslation->setEnabled(false);
            m_xReset->setEnabled(false);
            m_yReset->setEnabled(false);
            m_zReset->setEnabled(false);
        }

        m_visualFeedback->setChecked(m_currentLight->isVisualFeedbackOn());

        m_thetaSlider->setValue(static_cast<int>(m_currentLight->getThetaOffset() +
                                                 ILight::s_OFFSET_RANGE / 2));
        m_phiSlider->setValue(static_cast<int>(m_currentLight->getPhiOffset() +
                                               ILight::s_OFFSET_RANGE / 2));

        ::Ogre::SceneNode* const root = m_currentLight->getLayer()->getSceneManager()->getRootSceneNode();
        const ::Ogre::Node* const lightNode = sight::viz::scene3d::helper::Scene::getNodeById(
            m_currentLight->getTransformId(), root);
        const ::Ogre::Vector3 currentPos = lightNode->getPosition();

        m_xTranslation->setValue(static_cast<int>(currentPos[0]));
        m_yTranslation->setValue(static_cast<int>(currentPos[1]));
        m_zTranslation->setValue(static_cast<int>(currentPos[2]));
    }
    else
    {
        m_diffuseColorBtn->setEnabled(false);
        m_specularColorBtn->setEnabled(false);
        m_thetaSlider->setEnabled(false);
        m_phiSlider->setEnabled(false);
        m_lightTypeBox->setEnabled(false);
        m_visualFeedback->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

::Ogre::ColourValue SLightEditor::editColor(const ::Ogre::ColourValue& _currentColor, const std::string& _title)
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    QColor qColor = QColorDialog::getColor(modules::ui::viz::helper::Utils::converOgreColorToQColor(_currentColor),
                                           container,
                                           _title.c_str());

    return modules::ui::viz::helper::Utils::convertQColorToOgreColor(qColor);
}

//------------------------------------------------------------------------------

} // namespace sight::modules::ui::viz
