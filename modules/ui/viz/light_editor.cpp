/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modules/ui/viz/helper/utils.hpp"

#include <core/com/slots.hxx>

#include <data/map.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/helper/scene.hpp>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

namespace sight::module::ui::viz
{

using sight::viz::scene3d::light_adaptor;
using sight::viz::scene3d::layer;

//------------------------------------------------------------------------------

static const core::com::slots::key_t EDIT_LIGHT_SLOT = "edit_light";

//------------------------------------------------------------------------------

light_editor::light_editor() noexcept
{
    new_slot(EDIT_LIGHT_SLOT, &light_editor::edit_light, this);
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

    const QString service_id = QString::fromStdString(base_id());

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    qt_container->get_qt_container()->setObjectName(service_id);

    m_light_name_label = new QLabel("No light selected");
    m_light_name_label->setAlignment(Qt::AlignHCenter);

    m_light_type_box = new QComboBox();
    m_light_type_box->addItems(
        QStringList()
        << light_adaptor::POINT_LIGHT.c_str()
        << light_adaptor::DIRECTIONAL_LIGHT.c_str()
    );
    m_light_type_box->setEnabled(false);

    m_visual_feedback = new QPushButton("Feedback");
    m_visual_feedback->setObjectName(service_id + "/" + m_visual_feedback->text());
    m_visual_feedback->setCheckable(true);
    m_visual_feedback->setEnabled(false);

    m_diffuse_color_btn = new QPushButton("Diffuse color");
    m_diffuse_color_btn->setObjectName(service_id + "/" + m_diffuse_color_btn->text());
    m_diffuse_color_btn->setEnabled(false);

    m_specular_color_btn = new QPushButton("Specular color");
    m_specular_color_btn->setObjectName(service_id + "/" + m_specular_color_btn->text());
    m_specular_color_btn->setEnabled(false);

    m_theta_slider = new QSlider(Qt::Horizontal);
    m_theta_slider = new QSlider(Qt::Horizontal);
    m_theta_slider->setObjectName(service_id + "/thetaSlider");
    m_theta_slider->setMinimum(0);
    m_theta_slider->setMaximum(light_adaptor::OFFSET_RANGE);
    m_theta_slider->setEnabled(false);

    m_phi_slider = new QSlider(Qt::Horizontal);
    m_phi_slider->setObjectName(service_id + "/phiSlider");
    m_phi_slider->setMinimum(0);
    m_phi_slider->setMaximum(light_adaptor::OFFSET_RANGE);
    m_phi_slider->setEnabled(false);

    m_x_translation = new QSlider(Qt::Horizontal);
    m_x_translation->setObjectName(service_id + "/xTranslation");
    m_x_translation->setMinimum(-2000);
    m_x_translation->setMaximum(2000);
    m_x_translation->setEnabled(false);

    m_y_translation = new QSlider(Qt::Horizontal);
    m_y_translation->setObjectName(service_id + "/yTranslation");
    m_y_translation->setMinimum(-2000);
    m_y_translation->setMaximum(2000);
    m_y_translation->setEnabled(false);

    m_z_translation = new QSlider(Qt::Horizontal);
    m_z_translation->setObjectName(service_id + "/zTranslation");
    m_z_translation->setMinimum(-2000);
    m_z_translation->setMaximum(2000);
    m_z_translation->setEnabled(false);

    m_x_label = new QLineEdit("X: 0");
    m_x_label->setObjectName(service_id + "/xLabel");
    m_x_label->setReadOnly(true);
    m_x_label->setMaximumWidth(70);
    m_y_label = new QLineEdit("Y: 0");
    m_y_label->setObjectName(service_id + "/yLabel");
    m_y_label->setReadOnly(true);
    m_y_label->setMaximumWidth(70);
    m_z_label = new QLineEdit("Z: 0");
    m_z_label->setObjectName(service_id + "/zLabel");
    m_z_label->setReadOnly(true);
    m_z_label->setMaximumWidth(70);

    m_x_reset = new QPushButton("Reset");
    m_x_reset->setObjectName(service_id + "/xReset");
    m_x_reset->setEnabled(false);
    m_y_reset = new QPushButton("Reset");
    m_y_reset->setObjectName(service_id + "/yReset");
    m_y_reset->setEnabled(false);
    m_z_reset = new QPushButton("Reset");
    m_z_reset->setObjectName(service_id + "/zReset");
    m_z_reset->setEnabled(false);

    // Name of the selected light and its type
    auto* layout = new QVBoxLayout();
    layout->addWidget(m_light_name_label);

    auto* const type_layout = new QHBoxLayout();
    type_layout->addWidget(m_light_type_box);
    type_layout->addWidget(m_visual_feedback);
    layout->addLayout(type_layout);

    // Diffuse and specular colors
    auto* const color_layout = new QHBoxLayout();
    color_layout->addWidget(m_diffuse_color_btn);
    color_layout->addWidget(m_specular_color_btn);
    layout->addLayout(color_layout);

    // Theta offset
    auto* const theta_layout = new QHBoxLayout();
    theta_layout->addWidget(new QLabel("Theta offset :"));
    theta_layout->addWidget(m_theta_slider);
    layout->addLayout(theta_layout);

    // Phi offset
    auto* const phi_layout = new QHBoxLayout();
    phi_layout->addWidget(new QLabel("Phi offset :"));
    phi_layout->addWidget(m_phi_slider);
    layout->addLayout(phi_layout);

    // Translations;
    auto* const x_transformation_layout = new QHBoxLayout();
    x_transformation_layout->addWidget(m_x_label, 0);
    x_transformation_layout->addWidget(m_x_translation, 1);
    x_transformation_layout->addWidget(m_x_reset, 0);
    layout->addLayout(x_transformation_layout);

    auto* const y_transformation_layout = new QHBoxLayout();
    y_transformation_layout->addWidget(m_y_label, 0);
    y_transformation_layout->addWidget(m_y_translation, 1);
    y_transformation_layout->addWidget(m_y_reset, 0);
    layout->addLayout(y_transformation_layout);

    auto* const z_transformation_layout = new QHBoxLayout();
    z_transformation_layout->addWidget(m_z_label, 0);
    z_transformation_layout->addWidget(m_z_translation, 1);
    z_transformation_layout->addWidget(m_z_reset, 0);
    layout->addLayout(z_transformation_layout);

    qt_container->set_layout(layout);

    QObject::connect(m_diffuse_color_btn, &QPushButton::clicked, this, &light_editor::on_edit_diffuse_color);
    QObject::connect(m_specular_color_btn, &QPushButton::clicked, this, &light_editor::on_edit_specular_color);

    QObject::connect(m_theta_slider, &QSlider::valueChanged, this, &light_editor::on_edit_theta_offset);
    QObject::connect(m_phi_slider, &QSlider::valueChanged, this, &light_editor::on_edit_phi_offset);

    QObject::connect(m_light_type_box, &QComboBox::currentTextChanged, this, &light_editor::on_edit_type);

    QObject::connect(m_visual_feedback, &QPushButton::clicked, this, &light_editor::on_toggle_feedback);

    QObject::connect(m_x_translation, &QSlider::valueChanged, this, &light_editor::on_edit_x_translation);
    QObject::connect(m_y_translation, &QSlider::valueChanged, this, &light_editor::on_edit_y_translation);
    QObject::connect(m_z_translation, &QSlider::valueChanged, this, &light_editor::on_edit_z_translation);

    QObject::connect(m_x_reset, &QPushButton::clicked, this, &light_editor::on_reset_x_translation);
    QObject::connect(m_y_reset, &QPushButton::clicked, this, &light_editor::on_reset_y_translation);
    QObject::connect(m_z_reset, &QPushButton::clicked, this, &light_editor::on_reset_z_translation);
}

//------------------------------------------------------------------------------

void light_editor::updating()
{
}

//------------------------------------------------------------------------------

void light_editor::stopping()
{
    QObject::disconnect(m_diffuse_color_btn, &QPushButton::clicked, this, &light_editor::on_edit_diffuse_color);
    QObject::disconnect(m_specular_color_btn, &QPushButton::clicked, this, &light_editor::on_edit_specular_color);

    QObject::disconnect(m_theta_slider, &QSlider::valueChanged, this, &light_editor::on_edit_theta_offset);
    QObject::disconnect(m_phi_slider, &QSlider::valueChanged, this, &light_editor::on_edit_phi_offset);

    QObject::disconnect(m_light_type_box, &QComboBox::currentTextChanged, this, &light_editor::on_edit_type);

    QObject::disconnect(m_visual_feedback, &QPushButton::clicked, this, &light_editor::on_toggle_feedback);

    QObject::disconnect(m_x_translation, &QSlider::valueChanged, this, &light_editor::on_edit_x_translation);
    QObject::disconnect(m_y_translation, &QSlider::valueChanged, this, &light_editor::on_edit_y_translation);
    QObject::disconnect(m_z_translation, &QSlider::valueChanged, this, &light_editor::on_edit_z_translation);

    QObject::disconnect(m_x_reset, &QPushButton::clicked, this, &light_editor::on_reset_x_translation);
    QObject::disconnect(m_y_reset, &QPushButton::clicked, this, &light_editor::on_reset_y_translation);
    QObject::disconnect(m_z_reset, &QPushButton::clicked, this, &light_editor::on_reset_z_translation);

    this->destroy();
}

//------------------------------------------------------------------------------

void light_editor::on_edit_diffuse_color(bool /*unused*/)
{
    Ogre::ColourValue new_diffuse_color = this->edit_color(
        m_current_light->get_diffuse_color(),
        "Light diffuse color"
    );

    m_current_light->set_diffuse_color(new_diffuse_color);
}

//------------------------------------------------------------------------------

void light_editor::on_edit_specular_color(bool /*unused*/)
{
    Ogre::ColourValue new_specular_color = this->edit_color(
        m_current_light->get_specular_color(),
        "Light specular color"
    );

    m_current_light->set_specular_color(new_specular_color);
}

//------------------------------------------------------------------------------

void light_editor::on_edit_theta_offset(int _value)
{
    m_current_light->set_theta_offset(static_cast<float>(_value - light_adaptor::OFFSET_RANGE / 2.));
}

//------------------------------------------------------------------------------

void light_editor::on_edit_phi_offset(int _value)
{
    m_current_light->set_phi_offset(static_cast<float>(_value - light_adaptor::OFFSET_RANGE / 2.));
}

//------------------------------------------------------------------------------

void light_editor::on_edit_type(const QString& _type)
{
    if(_type == light_adaptor::POINT_LIGHT.c_str())
    {
        m_current_light->set_type(Ogre::Light::LT_POINT);
        m_theta_slider->setEnabled(false);
        m_phi_slider->setEnabled(false);
        if(m_current_light->get_name().find(layer::DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_x_translation->setEnabled(true);
            m_y_translation->setEnabled(true);
            m_z_translation->setEnabled(true);
            m_x_reset->setEnabled(true);
            m_y_reset->setEnabled(true);
            m_z_reset->setEnabled(true);
        }
    }
    else if(_type == light_adaptor::DIRECTIONAL_LIGHT.c_str())
    {
        m_current_light->set_type(Ogre::Light::LT_DIRECTIONAL);
        if(m_current_light->get_name().find(layer::DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_theta_slider->setEnabled(true);
            m_phi_slider->setEnabled(true);
        }

        m_x_translation->setEnabled(false);
        m_y_translation->setEnabled(false);
        m_z_translation->setEnabled(false);
        m_x_reset->setEnabled(false);
        m_y_reset->setEnabled(false);
        m_z_reset->setEnabled(false);
    }
    else
    {
        SIGHT_ASSERT("Unknow type for light", false);
    }

    m_current_light->update();
}

//------------------------------------------------------------------------------

void light_editor::on_toggle_feedback(bool _enable)
{
    m_current_light->enable_visual_feedback(_enable);
    m_current_light->render_service()->request_render();
}

//------------------------------------------------------------------------------

void light_editor::on_edit_x_translation(int _value)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(static_cast<Ogre::Real>(_value), current_pos[1], current_pos[2]));
    m_current_light->render_service()->request_render();

    m_x_label->setText(QString("X: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::on_edit_y_translation(int _value)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(current_pos[0], static_cast<Ogre::Real>(_value), current_pos[2]));
    m_current_light->render_service()->request_render();

    m_y_label->setText(QString("Y: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::on_edit_z_translation(int _value)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(current_pos[0], current_pos[1], static_cast<Ogre::Real>(_value)));
    m_current_light->render_service()->request_render();

    m_z_label->setText(QString("Z: %1").arg(_value));
}

//------------------------------------------------------------------------------

void light_editor::on_reset_x_translation(bool /*unused*/)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(0.F, current_pos[1], current_pos[2]));
    m_current_light->render_service()->request_render();

    m_x_label->setText("X: 0");
    m_x_translation->setValue(0);
}

//------------------------------------------------------------------------------

void light_editor::on_reset_y_translation(bool /*unused*/)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(current_pos[0], 0.F, current_pos[2]));
    m_current_light->render_service()->request_render();

    m_y_label->setText("Y: 0");
    m_y_translation->setValue(0);
}

//------------------------------------------------------------------------------

void light_editor::on_reset_z_translation(bool /*unused*/)
{
    Ogre::Node* const light_node    = this->get_light_node();
    const Ogre::Vector3 current_pos = light_node->getPosition();
    light_node->setPosition(Ogre::Vector3(current_pos[0], current_pos[1], 0.F));
    m_current_light->render_service()->request_render();

    m_z_label->setText("Z: 0");
    m_z_translation->setValue(0);
}

//------------------------------------------------------------------------------

Ogre::Node* light_editor::get_light_node() const
{
    Ogre::SceneNode* const root  = m_current_light->layer()->get_scene_manager()->getRootSceneNode();
    Ogre::Node* const light_node =
        sight::viz::scene3d::helper::scene::get_node_by_id(m_current_light->get_transform_id(), root);
    return light_node;
}

//------------------------------------------------------------------------------

void light_editor::edit_light(light_adaptor::sptr _light_adaptor)
{
    m_current_light = _light_adaptor;
    if(_light_adaptor)
    {
        SIGHT_ASSERT("The selected light adaptor doesn't exist.", _light_adaptor);

        m_light_name_label->setText(m_current_light->get_name().c_str());
        m_light_type_box->setCurrentIndex(static_cast<int>(m_current_light->type()));

        m_diffuse_color_btn->setEnabled(true);
        m_specular_color_btn->setEnabled(true);
        m_light_type_box->setEnabled(true);

        if(m_current_light->get_name().find(layer::DEFAULT_LIGHT_NAME) == std::string::npos)
        {
            m_visual_feedback->setEnabled(true);
            if(m_current_light->type() == Ogre::Light::LT_DIRECTIONAL)
            {
                m_theta_slider->setEnabled(true);
                m_phi_slider->setEnabled(true);
            }
            else if(m_current_light->type() == Ogre::Light::LT_POINT)
            {
                m_x_translation->setEnabled(true);
                m_y_translation->setEnabled(true);
                m_z_translation->setEnabled(true);
                m_x_reset->setEnabled(true);
                m_y_reset->setEnabled(true);
                m_z_reset->setEnabled(true);
            }
            else
            {
                SIGHT_ASSERT("Unknow type for light", false);
            }
        }
        else
        {
            m_visual_feedback->setEnabled(false);
            m_theta_slider->setEnabled(false);
            m_phi_slider->setEnabled(false);
            m_x_translation->setEnabled(false);
            m_y_translation->setEnabled(false);
            m_z_translation->setEnabled(false);
            m_x_reset->setEnabled(false);
            m_y_reset->setEnabled(false);
            m_z_reset->setEnabled(false);
        }

        m_visual_feedback->setChecked(m_current_light->is_visual_feedback_on());

        m_theta_slider->setValue(
            static_cast<int>(m_current_light->get_theta_offset()
                             + float(light_adaptor::OFFSET_RANGE / 2.))
        );
        m_phi_slider->setValue(
            static_cast<int>(m_current_light->get_phi_offset()
                             + float(light_adaptor::OFFSET_RANGE / 2.))
        );

        Ogre::SceneNode* const root        = m_current_light->layer()->get_scene_manager()->getRootSceneNode();
        const Ogre::Node* const light_node = sight::viz::scene3d::helper::scene::get_node_by_id(
            m_current_light->get_transform_id(),
            root
        );
        const Ogre::Vector3 current_pos = light_node->getPosition();

        m_x_translation->setValue(static_cast<int>(current_pos[0]));
        m_y_translation->setValue(static_cast<int>(current_pos[1]));
        m_z_translation->setValue(static_cast<int>(current_pos[2]));
    }
    else
    {
        m_diffuse_color_btn->setEnabled(false);
        m_specular_color_btn->setEnabled(false);
        m_theta_slider->setEnabled(false);
        m_phi_slider->setEnabled(false);
        m_light_type_box->setEnabled(false);
        m_visual_feedback->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

Ogre::ColourValue light_editor::edit_color(const Ogre::ColourValue& _current_color, const std::string& _title)
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* const container = qt_container->get_qt_container();

    QColor q_color = QColorDialog::getColor(
        module::ui::viz::helper::utils::convert_ogre_color_to_q_color(_current_color),
        container,
        _title.c_str()
    );

    return module::ui::viz::helper::utils::convert_q_color_to_ogre_color(q_color);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
