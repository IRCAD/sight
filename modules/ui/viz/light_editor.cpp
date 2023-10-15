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

#include "light_editor.hpp"

#include "modules/ui/viz/helper/Utils.hpp"

#include <core/com/slots.hxx>

#include <data/composite.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/helper/Scene.hpp>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

namespace sight::module::ui::viz
{

using sight::viz::scene3d::ILight;
using sight::viz::scene3d::Layer;

//------------------------------------------------------------------------------

static const core::com::slots::key_t EDIT_LIGHT_SLOT = "editLight";

//------------------------------------------------------------------------------

light_editor::light_editor() noexcept
{
    new_slot(EDIT_LIGHT_SLOT, &light_editor::editLight, this);
}

//------------------------------------------------------------------------------

light_editor::~light_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void light_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void light_editor::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    const auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_lightNameLabel = new QLabel("No light selected");
    m_lightNameLabel->setAlignment(Qt::AlignHCenter);

    m_lightTypeBox = new QComboBox();
    m_lightTypeBox->addItems(
        QStringList()
        << ILight::s_POINT_LIGHT.c_str()
        << ILight::s_DIRECTIONAL_LIGHT.c_str()
    );
    m_lightTypeBox->setEnabled(false);

    m_visualFeedback = new QPushButton("Feedback");
    m_visualFeedback->setObjectName(serviceID + "/" + m_visualFeedback->text());
    m_visualFeedback->setCheckable(true);
    m_visualFeedback->setEnabled(false);

    m_diffuseColorBtn = new QPushButton("Diffuse color");
    m_diffuseColorBtn->setObjectName(serviceID + "/" + m_diffuseColorBtn->text());
    m_diffuseColorBtn->setEnabled(false);

    m_specularColorBtn = new QPushButton("Specular color");
    m_specularColorBtn->setObjectName(serviceID + "/" + m_specularColorBtn->text());
    m_specularColorBtn->setEnabled(false);

    m_thetaSlider = new QSlider(Qt::Horizontal);
    m_thetaSlider = new QSlider(Qt::Horizontal);
    m_thetaSlider->setObjectName(serviceID + "/thetaSlider");
    m_thetaSlider->setMinimum(0);
    m_thetaSlider->setMaximum(ILight::s_OFFSET_RANGE);
    m_thetaSlider->setEnabled(false);

    m_phiSlider = new QSlider(Qt::Horizontal);
    m_phiSlider->setObjectName(serviceID + "/phiSlider");
    m_phiSlider->setMinimum(0);
    m_phiSlider->setMaximum(ILight::s_OFFSET_RANGE);
    m_phiSlider->setEnabled(false);

    m_xTranslation = new QSlider(Qt::Horizontal);
    m_xTranslation->setObjectName(serviceID + "/xTranslation");
    m_xTranslation->setMinimum(-2000);
    m_xTranslation->setMaximum(2000);
    m_xTranslation->setEnabled(false);

    m_yTranslation = new QSlider(Qt::Horizontal);
    m_yTranslation->setObjectName(serviceID + "/yTranslation");
    m_yTranslation->setMinimum(-2000);
    m_yTranslation->setMaximum(2000);
    m_yTranslation->setEnabled(false);

    m_zTranslation = new QSlider(Qt::Horizontal);
    m_zTranslation->setObjectName(serviceID + "/zTranslation");
    m_zTranslation->setMinimum(-2000);
    m_zTranslation->setMaximum(2000);
    m_zTranslation->setEnabled(false);

    m_xLabel = new QLineEdit("X: 0");
    m_xLabel->setObjectName(serviceID + "/xLabel");
    m_xLabel->setReadOnly(true);
    m_xLabel->setMaximumWidth(70);
    m_yLabel = new QLineEdit("Y: 0");
    m_yLabel->setObjectName(serviceID + "/yLabel");
    m_yLabel->setReadOnly(true);
    m_yLabel->setMaximumWidth(70);
    m_zLabel = new QLineEdit("Z: 0");
    m_zLabel->setObjectName(serviceID + "/zLabel");
    m_zLabel->setReadOnly(true);
    m_zLabel->setMaximumWidth(70);

    m_xReset = new QPushButton("Reset");
    m_xReset->setObjectName(serviceID + "/xReset");
    m_xReset->setEnabled(false);
    m_yReset = new QPushButton("Reset");
    m_yReset->setObjectName(serviceID + "/yReset");
    m_yReset->setEnabled(false);
    m_zReset = new QPushButton("Reset");
    m_zReset->setObjectName(serviceID + "/zReset");
    m_zReset->setEnabled(false);

    // Name of the selected light and its type
    auto* layout = new QVBoxLayout();
    layout->addWidget(m_lightNameLabel);

    auto* const typeLayout = new QHBoxLayout();
    typeLayout->addWidget(m_lightTypeBox);
    typeLayout->addWidget(m_visualFeedback);
    layout->addLayout(typeLayout);

    // Diffuse and specular colors
    auto* const colorLayout = new QHBoxLayout();
    colorLayout->addWidget(m_diffuseColorBtn);
    colorLayout->addWidget(m_specularColorBtn);
    layout->addLayout(colorLayout);

    // Theta offset
    auto* const thetaLayout = new QHBoxLayout();
    thetaLayout->addWidget(new QLabel("Theta offset :"));
    thetaLayout->addWidget(m_thetaSlider);
    layout->addLayout(thetaLayout);

    // Phi offset
    auto* const phiLayout = new QHBoxLayout();
    phiLayout->addWidget(new QLabel("Phi offset :"));
    phiLayout->addWidget(m_phiSlider);
    layout->addLayout(phiLayout);

    // Translations;
    auto* const xTransformationLayout = new QHBoxLayout();
    xTransformationLayout->addWidget(m_xLabel, 0);
    xTransformationLayout->addWidget(m_xTranslation, 1);
    xTransformationLayout->addWidget(m_xReset, 0);
    layout->addLayout(xTransformationLayout);

    auto* const yTransformationLayout = new QHBoxLayout();
    yTransformationLayout->addWidget(m_yLabel, 0);
    yTransformationLayout->addWidget(m_yTranslation, 1);
    yTransformationLayout->addWidget(m_yReset, 0);
    layout->addLayout(yTransformationLayout);

    auto* const zTransformationLayout = new QHBoxLayout();
    zTransformationLayout->addWidget(m_zLabel, 0);
    zTransformationLayout->addWidget(m_zTranslation, 1);
    zTransformationLayout->addWidget(m_zReset, 0);
    layout->addLayout(zTransformationLayout);

    qtContainer->setLayout(layout);

    QObject::connect(m_diffuseColorBtn, &QPushButton::clicked, this, &light_editor::onEditDiffuseColor);
    QObject::connect(m_specularColorBtn, &QPushButton::clicked, this, &light_editor::onEditSpecularColor);

    QObject::connect(m_thetaSlider, &QSlider::valueChanged, this, &light_editor::onEditThetaOffset);
    QObject::connect(m_phiSlider, &QSlider::valueChanged, this, &light_editor::onEditPhiOffset);

    QObject::connect(m_lightTypeBox, &QComboBox::currentTextChanged, this, &light_editor::onEditType);

    QObject::connect(m_visualFeedback, &QPushButton::clicked, this, &light_editor::onToggleFeedback);

    QObject::connect(m_xTranslation, &QSlider::valueChanged, this, &light_editor::onEditXTranslation);
    QObject::connect(m_yTranslation, &QSlider::valueChanged, this, &light_editor::onEditYTranslation);
    QObject::connect(m_zTranslation, &QSlider::valueChanged, this, &light_editor::onEditZTranslation);

    QObject::connect(m_xReset, &QPushButton::clicked, this, &light_editor::onResetXTranslation);
    QObject::connect(m_yReset, &QPushButton::clicked, this, &light_editor::onResetYTranslation);
    QObject::connect(m_zReset, &QPushButton::clicked, this, &light_editor::onResetZTranslation);
}

//------------------------------------------------------------------------------

void light_editor::updating()
{
}

//------------------------------------------------------------------------------

void light_editor::stopping()
{
    QObject::disconnect(m_diffuseColorBtn, &QPushButton::clicked, this, &light_editor::onEditDiffuseColor);
    QObject::disconnect(m_specularColorBtn, &QPushButton::clicked, this, &light_editor::onEditSpecularColor);

    QObject::disconnect(m_thetaSlider, &QSlider::valueChanged, this, &light_editor::onEditThetaOffset);
    QObject::disconnect(m_phiSlider, &QSlider::valueChanged, this, &light_editor::onEditPhiOffset);

    QObject::disconnect(m_lightTypeBox, &QComboBox::currentTextChanged, this, &light_editor::onEditType);

    QObject::disconnect(m_visualFeedback, &QPushButton::clicked, this, &light_editor::onToggleFeedback);

    QObject::disconnect(m_xTranslation, &QSlider::valueChanged, this, &light_editor::onEditXTranslation);
    QObject::disconnect(m_yTranslation, &QSlider::valueChanged, this, &light_editor::onEditYTranslation);
    QObject::disconnect(m_zTranslation, &QSlider::valueChanged, this, &light_editor::onEditZTranslation);

    QObject::disconnect(m_xReset, &QPushButton::clicked, this, &light_editor::onResetXTranslation);
    QObject::disconnect(m_yReset, &QPushButton::clicked, this, &light_editor::onResetYTranslation);
    QObject::disconnect(m_zReset, &QPushButton::clicked, this, &light_editor::onResetZTranslation);

    this->destroy();
}

//------------------------------------------------------------------------------

void light_editor::onEditDiffuseColor(bool /*unused*/)
{
    Ogre::ColourValue newDiffuseColor = this->editColor(
        m_currentLight->getDiffuseColor(),
        "Light diffuse color"
    );

    m_currentLight->setDiffuseColor(newDiffuseColor);
}

//------------------------------------------------------------------------------

void light_editor::onEditSpecularColor(bool /*unused*/)
{
    Ogre::ColourValue newSpecularColor = this->editColor(
        m_currentLight->getSpecularColor(),
        "Light specular color"
    );

    m_currentLight->setSpecularColor(newSpecularColor);
}

//------------------------------------------------------------------------------

void light_editor::onEditThetaOffset(int _value)
{
    m_currentLight->setThetaOffset(static_cast<float>(_value - ILight::s_OFFSET_RANGE / 2.));
}

//------------------------------------------------------------------------------

void light_editor::onEditPhiOffset(int _value)
{
    m_currentLight->setPhiOffset(static_cast<float>(_value - ILight::s_OFFSET_RANGE / 2.));
}

//------------------------------------------------------------------------------

void light_editor::onEditType(const QString& _type)
{
    if(_type == ILight::s_POINT_LIGHT.c_str())
    {
        m_currentLight->setType(Ogre::Light::LT_POINT);
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
        m_currentLight->setType(Ogre::Light::LT_DIRECTIONAL);
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
        SIGHT_ASSERT("Unknow type for light", false);
    }

    m_currentLight->update();
}

//------------------------------------------------------------------------------

void light_editor::onToggleFeedback(bool _enable)
{
    m_currentLight->enableVisualFeedback(_enable);
    m_currentLight->getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void light_editor::onEditXTranslation(int _value)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(static_cast<Ogre::Real>(_value), currentPos[1], currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_xLabel->setText(QString("X: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::onEditYTranslation(int _value)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(currentPos[0], static_cast<Ogre::Real>(_value), currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_yLabel->setText(QString("Y: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::onEditZTranslation(int _value)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(currentPos[0], currentPos[1], static_cast<Ogre::Real>(_value)));
    m_currentLight->getRenderService()->requestRender();

    m_zLabel->setText(QString("Z: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::onResetXTranslation(bool /*unused*/)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(0.F, currentPos[1], currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_xLabel->setText("X: 0");
    m_xTranslation->setValue(0);
}

//------------------------------------------------------------------------------

void light_editor::onResetYTranslation(bool /*unused*/)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(currentPos[0], 0.F, currentPos[2]));
    m_currentLight->getRenderService()->requestRender();

    m_yLabel->setText("Y: 0");
    m_yTranslation->setValue(0);
}

//------------------------------------------------------------------------------

void light_editor::onResetZTranslation(bool /*unused*/)
{
    Ogre::Node* const lightNode    = this->getLightNode();
    const Ogre::Vector3 currentPos = lightNode->getPosition();
    lightNode->setPosition(Ogre::Vector3(currentPos[0], currentPos[1], 0.F));
    m_currentLight->getRenderService()->requestRender();

    m_zLabel->setText("Z: 0");
    m_zTranslation->setValue(0);
}

//------------------------------------------------------------------------------

Ogre::Node* light_editor::getLightNode() const
{
    Ogre::SceneNode* const root = m_currentLight->getLayer()->getSceneManager()->getRootSceneNode();
    Ogre::Node* const lightNode =
        sight::viz::scene3d::helper::Scene::getNodeById(m_currentLight->getTransformId(), root);
    return lightNode;
}

//------------------------------------------------------------------------------

void light_editor::editLight(ILight::sptr _lightAdaptor)
{
    m_currentLight = _lightAdaptor;
    if(_lightAdaptor)
    {
        SIGHT_ASSERT("The selected light adaptor doesn't exist.", _lightAdaptor);

        m_lightNameLabel->setText(m_currentLight->getName().c_str());
        m_lightTypeBox->setCurrentIndex(static_cast<int>(m_currentLight->getType()));

        m_diffuseColorBtn->setEnabled(true);
        m_specularColorBtn->setEnabled(true);
        m_lightTypeBox->setEnabled(true);

        if(m_currentLight->getName().find(Layer::s_DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_visualFeedback->setEnabled(true);
            if(m_currentLight->getType() == Ogre::Light::LT_DIRECTIONAL)
            {
                m_thetaSlider->setEnabled(true);
                m_phiSlider->setEnabled(true);
            }
            else if(m_currentLight->getType() == Ogre::Light::LT_POINT)
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
                SIGHT_ASSERT("Unknow type for light", false);
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

        m_thetaSlider->setValue(
            static_cast<int>(m_currentLight->getThetaOffset()
                             + float(ILight::s_OFFSET_RANGE / 2.))
        );
        m_phiSlider->setValue(
            static_cast<int>(m_currentLight->getPhiOffset()
                             + float(ILight::s_OFFSET_RANGE / 2.))
        );

        Ogre::SceneNode* const root       = m_currentLight->getLayer()->getSceneManager()->getRootSceneNode();
        const Ogre::Node* const lightNode = sight::viz::scene3d::helper::Scene::getNodeById(
            m_currentLight->getTransformId(),
            root
        );
        const Ogre::Vector3 currentPos = lightNode->getPosition();

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

Ogre::ColourValue light_editor::editColor(const Ogre::ColourValue& _currentColor, const std::string& _title)
{
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qtContainer->getQtContainer();

    QColor qColor = QColorDialog::getColor(
        module::ui::viz::helper::Utils::convertOgreColorToQColor(_currentColor),
        container,
        _title.c_str()
    );

    return module::ui::viz::helper::Utils::convertQColorToOgreColor(qColor);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
