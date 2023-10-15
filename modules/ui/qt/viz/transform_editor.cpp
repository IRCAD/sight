/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "modules/ui/qt/viz/transform_editor.hpp"

#include <core/com/signal.hxx>

#include <geometry/data/matrix4.hpp>

#include <ui/qt/container/widget.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

#include <regex>

namespace sight::module::ui::qt::viz
{

//------------------------------------------------------------------------------

transform_editor::transform_editor() noexcept :
    m_rotation("xyz"),
    m_translation("xyz")
{
    m_translationRange[0] = -300;
    m_translationRange[1] = +300;
    m_rotationRange[0]    = -180;
    m_rotationRange[1]    = 180;
}

//------------------------------------------------------------------------------

transform_editor::~transform_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void transform_editor::configuring()
{
    static const std::regex s_REGEX("[xyz][xyz]?[xyz]?");

    this->initialize();

    service::config_t config = this->get_config();

    const std::string rotation = config.get<std::string>("rotation.<xmlattr>.enabled", "true");

    if(rotation == "false")
    {
        m_rotation = "";
    }
    else if(rotation == "true")
    {
        m_rotation = "xyz";
    }
    else if(std::regex_match(rotation, s_REGEX))
    {
        m_rotation = rotation;
    }
    else
    {
        SIGHT_ERROR("Attribute 'rotation' should be 'true', 'false' or a combination of [xyz]");
    }

    m_rotationRange[0] = config.get<int>("rotation.<xmlattr>.min", m_rotationRange[0]);
    m_rotationRange[1] = config.get<int>("rotation.<xmlattr>.max", m_rotationRange[1]);

    const std::string translation = config.get<std::string>("translation.<xmlattr>.enabled", "true");

    if(translation == "false")
    {
        m_translation = "";
    }
    else if(translation == "true")
    {
        m_translation = "xyz";
    }
    else if(std::regex_match(translation, s_REGEX))
    {
        m_translation = translation;
    }
    else
    {
        SIGHT_ERROR("Attribute 'translation' should be 'true', 'false' or a combination of [xyz]");
    }

    m_translationRange[0] = config.get<int>("translation.<xmlattr>.min", m_translationRange[0]);
    m_translationRange[1] = config.get<int>("translation.<xmlattr>.max", m_translationRange[1]);
}

//------------------------------------------------------------------------------

void transform_editor::starting()
{
    const std::array description {"Translation X", "Translation Y", "Translation Z",
                                  "Rotation X", "Rotation Y", "Rotation Z"
    };

    this->create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QVBoxLayout();

    qtContainer->setLayout(layout);

    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        auto* sliderLayout = new QHBoxLayout();

        m_sliders[i].m_sliderValue     = new QLineEdit();
        m_sliders[i].m_labelMin        = new QLabel();
        m_sliders[i].m_labelMax        = new QLabel();
        m_sliders[i].m_labelDefinition = new QLabel();

        m_sliders[i].m_slider = new QSlider(Qt::Horizontal);
        m_sliders[i].m_slider->setTickInterval(1);

        m_sliders[i].m_labelDefinition->setText(description[i]);

        sliderLayout->addWidget(m_sliders[i].m_labelDefinition, 0);
        sliderLayout->addWidget(m_sliders[i].m_labelMin, 0);
        sliderLayout->addWidget(m_sliders[i].m_slider, 3);
        sliderLayout->addWidget(m_sliders[i].m_labelMax, 0);
        sliderLayout->addWidget(m_sliders[i].m_sliderValue, 1);

        layout->addLayout(sliderLayout, 0);
        QObject::connect(m_sliders[i].m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
        QObject::connect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));
    }

    for(unsigned int i = POSITION_X ; i <= POSITION_Z ; i++)
    {
        m_sliders[i].m_labelMin->setText(std::to_string(m_translationRange[0]).c_str());
        m_sliders[i].m_labelMax->setText(std::to_string(m_translationRange[1]).c_str());
        m_sliders[i].m_slider->setRange(m_translationRange[0], m_translationRange[1]);
    }

    const std::string axes = "xyzxyz";

    for(unsigned int i = POSITION_X ; i <= POSITION_Z ; i++)
    {
        const bool visible = m_translation.find(axes[i]) != std::string::npos;
        m_sliders[i].m_sliderValue->setVisible(visible);
        m_sliders[i].m_labelMin->setVisible(visible);
        m_sliders[i].m_labelMax->setVisible(visible);
        m_sliders[i].m_labelDefinition->setVisible(visible);
        m_sliders[i].m_slider->setVisible(visible);
    }

    for(unsigned int i = ROTATION_X ; i <= ROTATION_Z ; i++)
    {
        m_sliders[i].m_labelMin->setText(std::to_string(m_rotationRange[0]).c_str());
        m_sliders[i].m_labelMax->setText(std::to_string(m_rotationRange[1]).c_str());
        m_sliders[i].m_slider->setRange(m_rotationRange[0], m_rotationRange[1]);
    }

    for(unsigned int i = ROTATION_X ; i <= ROTATION_Z ; i++)
    {
        const bool visible = m_rotation.find(axes[i]) != std::string::npos;
        m_sliders[i].m_sliderValue->setVisible(visible);
        m_sliders[i].m_labelMin->setVisible(visible);
        m_sliders[i].m_labelMax->setVisible(visible);
        m_sliders[i].m_labelDefinition->setVisible(visible);
        m_sliders[i].m_slider->setVisible(visible);
    }

    this->updateFromMatrix();
}

//------------------------------------------------------------------------------

void transform_editor::stopping()
{
    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        QObject::disconnect(m_sliders[i].m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
        QObject::disconnect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void transform_editor::updating()
{
    this->updateFromMatrix();
}

//-----------------------------------------------------------------------------

service::connections_t transform_editor::auto_connections() const
{
    connections_t connections;

    connections.push(s_MATRIX_INOUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void transform_editor::onSliderChanged(int /*unused*/)
{
    const auto rx = glm::radians<double>(m_sliders[ROTATION_X].m_slider->value());
    const auto ry = glm::radians<double>(m_sliders[ROTATION_Y].m_slider->value());
    const auto rz = glm::radians<double>(m_sliders[ROTATION_Z].m_slider->value());

    const double tx = m_sliders[POSITION_X].m_slider->value();
    const double ty = m_sliders[POSITION_Y].m_slider->value();
    const double tz = m_sliders[POSITION_Z].m_slider->value();

    glm::dquat quat  = glm::dquat(glm::dvec3(rx, ry, rz));
    glm::dmat4x4 mat = glm::mat4_cast(quat);

    mat[3] = glm::dvec4(tx, ty, tz, 1.);

    const auto matrix = m_matrix.lock();
    geometry::data::setTF3DFromMatrix(*matrix, mat);

    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        m_sliders[i].m_sliderValue->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
    }

    auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transform_editor::onTextChanged()
{
    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        QString string = m_sliders[i].m_sliderValue->text();
        m_sliders[i].m_slider->setValue(string.toInt());
    }
}

//------------------------------------------------------------------------------

void transform_editor::updateFromMatrix()
{
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("Unable to get matrix", matrix);

    const glm::dmat4x4 mat = geometry::data::getMatrixFromTF3D(*matrix);

    const glm::dquat quat(mat);
    const glm::dvec3 angles = glm::eulerAngles(quat);

    const glm::dvec4 translation = mat[3];

    // Block
    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        m_sliders[i].m_slider->blockSignals(true);
        m_sliders[i].m_sliderValue->blockSignals(true);
    }

    for(glm::length_t i = POSITION_X, j = 0 ; i <= POSITION_Z ; i++, ++j)
    {
        m_sliders[unsigned(i)].m_slider->setValue(static_cast<int>(translation[j]));
    }

    for(glm::length_t i = ROTATION_X, j = 0 ; i <= ROTATION_Z ; i++, ++j)
    {
        m_sliders[unsigned(i)].m_slider->setValue(static_cast<int>(glm::degrees<double>(angles[j])));
    }

    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        m_sliders[i].m_sliderValue->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
    }

    for(unsigned int i = 0 ; i < MAX_SLIDER_INDEX ; i++)
    {
        m_sliders[i].m_slider->blockSignals(false);
        m_sliders[i].m_sliderValue->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
