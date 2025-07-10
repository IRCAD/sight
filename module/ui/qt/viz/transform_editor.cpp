/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

#include "module/ui/qt/viz/transform_editor.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

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
    m_translation_range[0] = -300;
    m_translation_range[1] = +300;
    m_rotation_range[0]    = -180;
    m_rotation_range[1]    = 180;

    new_slot(slots::SET_TRANSLATION_RANGE, &transform_editor::set_translation_range, this);
}

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

    m_rotation_range[0] = config.get<int>("rotation.<xmlattr>.min", m_rotation_range[0]);
    m_rotation_range[1] = config.get<int>("rotation.<xmlattr>.max", m_rotation_range[1]);

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

    m_translation_range[0] = config.get<int>("translation.<xmlattr>.min", m_translation_range[0]);
    m_translation_range[1] = config.get<int>("translation.<xmlattr>.max", m_translation_range[1]);
}

//------------------------------------------------------------------------------

void transform_editor::starting()
{
    const std::array description {"Translation X", "Translation Y", "Translation Z",
                                  "Rotation X", "Rotation Y", "Rotation Z"
    };

    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* layout = new QVBoxLayout();

    qt_container->set_layout(layout);

    const auto minimal = *m_minimal;

    for(unsigned int i = 0 ; i < max_slider_index ; i++)
    {
        auto* slider_layout = new QHBoxLayout();

        m_sliders[i].m_slider = new QSlider(Qt::Horizontal);
        m_sliders[i].m_slider->setTickInterval(1);

        if(not minimal)
        {
            m_sliders[i].m_slider_value     = new QLineEdit();
            m_sliders[i].m_label_min        = new QLabel();
            m_sliders[i].m_label_max        = new QLabel();
            m_sliders[i].m_label_definition = new QLabel();

            m_sliders[i].m_label_definition->setText(description[i]);
            slider_layout->addWidget(m_sliders[i].m_label_definition, 0);
            slider_layout->addWidget(m_sliders[i].m_label_min, 0);
        }

        slider_layout->addWidget(m_sliders[i].m_slider, 3);

        if(not minimal)
        {
            slider_layout->addWidget(m_sliders[i].m_label_max, 0);
            slider_layout->addWidget(m_sliders[i].m_slider_value, 1);
            QObject::connect(m_sliders[i].m_slider_value, &QLineEdit::editingFinished, this, &self_t::on_text_changed);
        }

        layout->addLayout(slider_layout, 0);
        QObject::connect(m_sliders[i].m_slider, &QSlider::valueChanged, this, &self_t::on_slider_changed);
    }

    for(unsigned int i = position_x ; i <= position_z ; i++)
    {
        if(not minimal)
        {
            m_sliders[i].m_label_min->setText(QString("%1").arg(m_translation_range[0]));
            m_sliders[i].m_label_max->setText(QString("%1").arg(m_translation_range[1]));
        }

        m_sliders[i].m_slider->setRange(m_translation_range[0], m_translation_range[1]);
    }

    const std::string axes = "xyzxyz";

    for(unsigned int i = position_x ; i <= position_z ; i++)
    {
        const bool visible = m_translation.find(axes[i]) != std::string::npos;

        if(not minimal)
        {
            m_sliders[i].m_slider_value->setVisible(visible);
            m_sliders[i].m_label_min->setVisible(visible);
            m_sliders[i].m_label_max->setVisible(visible);
            m_sliders[i].m_label_definition->setVisible(visible);
        }

        m_sliders[i].m_slider->setVisible(visible);
    }

    for(unsigned int i = rotation_x ; i <= rotation_z ; i++)
    {
        if(not minimal)
        {
            m_sliders[i].m_label_min->setText(QString("%1").arg(m_rotation_range[0]));
            m_sliders[i].m_label_max->setText(QString("%1").arg(m_rotation_range[1]));
        }

        m_sliders[i].m_slider->setRange(m_rotation_range[0], m_rotation_range[1]);
    }

    for(unsigned int i = rotation_x ; i <= rotation_z ; i++)
    {
        const bool visible = m_rotation.find(axes[i]) != std::string::npos;
        if(not minimal)
        {
            m_sliders[i].m_slider_value->setVisible(visible);
            m_sliders[i].m_label_min->setVisible(visible);
            m_sliders[i].m_label_max->setVisible(visible);
            m_sliders[i].m_label_definition->setVisible(visible);
        }

        m_sliders[i].m_slider->setVisible(visible);
    }

    this->update_from_matrix();
}

//------------------------------------------------------------------------------

void transform_editor::stopping()
{
    const auto minimal = *m_minimal;
    for(unsigned int i = 0 ; i < max_slider_index ; i++)
    {
        QObject::disconnect(
            m_sliders[i].m_slider,
            &QSlider::valueChanged,
            this,
            &self_t::on_slider_changed
        );

        if(not minimal)
        {
            QObject::disconnect(
                m_sliders[i].m_slider_value,
                &QLineEdit::editingFinished,
                this,
                &self_t::on_text_changed
            );
        }
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void transform_editor::updating()
{
    this->update_from_matrix();
}

//-----------------------------------------------------------------------------

service::connections_t transform_editor::auto_connections() const
{
    connections_t connections;

    connections.push(MATRIX_INOUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void transform_editor::set_translation_range(double _min, double _max)
{
    if(_min != std::numeric_limits<double>::max())
    {
        m_translation_range[0] = static_cast<int>(_min);
    }

    if(_max != std::numeric_limits<double>::min())
    {
        m_translation_range[1] = static_cast<int>(_max);
    }

    const auto minimal = *m_minimal;

    for(unsigned int i = position_x ; i <= position_z ; i++)
    {
        if(m_sliders[i].m_slider->isVisible())
        {
            const auto matrix = m_matrix.lock();
            SIGHT_ASSERT("Unable to get matrix", matrix);
            const auto value = std::clamp((*matrix)[3 + static_cast<size_t>(i) * 4], _min, _max);

            m_sliders[i].m_slider->blockSignals(true);
            if(not minimal)
            {
                m_sliders[i].m_label_min->setText(QString("%1").arg(m_translation_range[0]));
                m_sliders[i].m_label_max->setText(QString("%1").arg(m_translation_range[1]));
                m_sliders[i].m_slider_value->setText(QString("%1").arg(value));
            }

            (*matrix)[3 + static_cast<size_t>(i) * 4] = value;
            m_sliders[i].m_slider->setValue(static_cast<int>(value));
            m_sliders[i].m_slider->setRange(m_translation_range[0], m_translation_range[1]);
            m_sliders[i].m_slider->blockSignals(false);
            if(_min == _max)
            {
                m_sliders[i].m_slider->setEnabled(false);
            }
            else
            {
                m_sliders[i].m_slider->setEnabled(true);
            }

            // matrix->async_emit(this, sight::data::object::MODIFIED_SIG);
        }
    }
}

//------------------------------------------------------------------------------

void transform_editor::on_slider_changed(int /*unused*/)
{
    const auto rx = glm::radians<double>(m_sliders[rotation_x].m_slider->value());
    const auto ry = glm::radians<double>(m_sliders[rotation_y].m_slider->value());
    const auto rz = glm::radians<double>(m_sliders[rotation_z].m_slider->value());

    const double tx = m_sliders[position_x].m_slider->value();
    const double ty = m_sliders[position_y].m_slider->value();
    const double tz = m_sliders[position_z].m_slider->value();

    const glm::dquat quat = glm::dquat(glm::dvec3(rx, ry, rz));
    glm::dmat4x4 mat      = glm::mat4_cast(quat);
    mat = glm::translate(mat, glm::dvec3(tx, ty, tz));

    const auto matrix = m_matrix.lock();
    geometry::data::from_glm_mat(*matrix, mat);

    const auto minimal = *m_minimal;
    if(not minimal)
    {
        for(unsigned int i = 0 ; i < max_slider_index ; i++)
        {
            m_sliders[i].m_slider_value->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
        }
    }

    matrix->async_emit(this, data::object::MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void transform_editor::on_text_changed()
{
    for(unsigned int i = 0 ; i < max_slider_index ; i++)
    {
        QString string = m_sliders[i].m_slider_value->text();
        m_sliders[i].m_slider->setValue(string.toInt());
    }
}

//------------------------------------------------------------------------------

void transform_editor::update_from_matrix()
{
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("Unable to get matrix", matrix);

    const glm::dmat4x4 mat = geometry::data::to_glm_mat(*matrix);

    const glm::dquat quat(mat);
    const glm::dvec3 angles = glm::eulerAngles(quat);

    const glm::dvec4 translation = mat[3];

    const auto minimal = *m_minimal;
    // Block
    for(unsigned int i = 0 ; i < max_slider_index ; i++)
    {
        m_sliders[i].m_slider->blockSignals(true);
        if(not minimal)
        {
            m_sliders[i].m_slider_value->blockSignals(true);
        }
    }

    for(glm::length_t i = position_x, j = 0 ; i <= position_z ; i++, ++j)
    {
        m_sliders[unsigned(i)].m_slider->setValue(static_cast<int>(translation[j]));
    }

    for(glm::length_t i = rotation_x, j = 0 ; i <= rotation_z ; i++, ++j)
    {
        m_sliders[unsigned(i)].m_slider->setValue(static_cast<int>(glm::degrees<double>(angles[j])));
    }

    if(not minimal)
    {
        for(unsigned int i = 0 ; i < max_slider_index ; i++)
        {
            m_sliders[i].m_slider_value->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
            m_sliders[i].m_slider_value->blockSignals(false);
        }
    }

    for(unsigned int i = 0 ; i < max_slider_index ; i++)
    {
        m_sliders[i].m_slider->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
