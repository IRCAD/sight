/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "module/ui/qt/image/slice_index_position_editor.hpp"

#include "data/image_series.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/spy_log.hpp>

#include <data/helper/fiducials_series.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>

#include <geometry/data/image.hpp>

#include <service/macros.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

namespace sight::module::ui::qt::image
{

namespace medical_image = data::helper::medical_image;

static const core::com::slots::key_t UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

const service::base::key_t slice_index_position_editor::IMAGE_INOUT =
    "image";

std::map<slice_index_position_editor::axis_t, std::string> slice_index_position_editor::orientation_prefix_map = {
    {slice_index_position_editor::axis_t::axial, "S"},
    {slice_index_position_editor::axis_t::frontal, "P"},
    {slice_index_position_editor::axis_t::sagittal, "L"}
};

//------------------------------------------------------------------------------

slice_index_position_editor::slice_index_position_editor() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &slice_index_position_editor::update_slice_index, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &slice_index_position_editor::update_slice_type, this);
}

//----------------------------------------------------------------------------------------
void slice_index_position_editor::configuring()
{
    this->initialize();

    const config_t config             = this->get_config();
    static const std::string s_CONFIG = "config.<xmlattr>.";

    static const std::string s_SLICE_INDEX_CONFIG = s_CONFIG + "orientation";
    static const std::string s_SLICE_LABEL_CONFIG = s_CONFIG + "label";
    static const std::string s_AXIS_CONFIG        = s_CONFIG + "display_axis_selector";
    static const std::string s_STEP_CONFIG        = s_CONFIG + "display_step_buttons";

    auto orientation  = config.get<std::string>(s_SLICE_INDEX_CONFIG, "axial");
    auto label_option = config.get<std::string>(s_SLICE_LABEL_CONFIG, "index");

    m_display_axis_selector = config.get<bool>(s_AXIS_CONFIG, m_display_axis_selector);

    m_display_step_buttons = config.get<bool>(s_STEP_CONFIG, m_display_step_buttons);

    if(label_option == "position")
    {
        m_label_option = label_option_t::position;
    }
    else if(label_option == "index")
    {
        m_label_option = label_option_t::index;
    }
    else
    {
        SIGHT_FATAL("The value for the xml element \"label\" can only be position or index.");
    }

    if(orientation == "axial")
    {
        m_axis = axis_t::axial; // Z
    }
    else if(orientation == "frontal")
    {
        m_axis = axis_t::frontal; // Y
    }
    else if(orientation == "sagittal")
    {
        m_axis = axis_t::sagittal; // X
    }
    else
    {
        SIGHT_FATAL("The value for the xml element \"orientation\" can only be axial, frontal or sagittal.");
    }
}

//-------------------------------------------------------------------------------------------
void slice_index_position_editor::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    const QString service_id = QString::fromStdString(base_id());

    auto* layout = new QVBoxLayout();
    sight::ui::qt::slice_selector::ChangeIndexCallback fct_index = [this](int _index)
                                                                   {
                                                                       slice_index_notification(_index);
                                                                   };
    sight::ui::qt::slice_selector::ChangeTypeCallback fct_type = [this](int _type){slice_type_notification(_type);};
    sight::ui::qt::slice_selector::ChangeLabelCallback fct_label = [this](){slice_label_notification();};

    if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            static_cast<std::uint8_t>(1)

        );
    }
    else if(m_label_option == label_option_t::position)
    {
        m_slice_selector_with_index = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            0.0

        );
    }

    m_slice_selector_with_index->setProperty("class", "slice_selector");
    m_slice_selector_with_index->set_enabled(false);
    m_slice_selector_with_index->setObjectName(service_id);

    m_slice_selector_with_index->set_change_index_callback(fct_index);
    m_slice_selector_with_index->set_change_type_callback(fct_type);
    m_slice_selector_with_index->set_change_label_callback(fct_label);

    m_slice_selector_with_index->set_type_selection(m_axis);

    layout->addWidget(m_slice_selector_with_index);

    m_slice_selector_with_index->set_prefix(orientation_prefix_map.at(m_axis));

    layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updating()
{
    {
        const auto image = m_image.const_lock();

        if(m_label_option == label_option_t::index)
        {
            const bool image_is_valid = medical_image::check_image_validity(image.get_shared());

            m_slice_selector_with_index->set_enable(image_is_valid);

            m_axial_index = std::max(
                std::int64_t(0),
                medical_image::get_slice_index(*image, axis_t::axial).value_or(0)
            );
            m_frontal_index = std::max(
                std::int64_t(0),
                medical_image::get_slice_index(*image, axis_t::frontal).value_or(0)
            );
            m_sagittal_index = std::max(
                std::int64_t(0),
                medical_image::get_slice_index(*image, axis_t::sagittal).value_or(0)
            );

            this->update_slice_index_from_img(*image);
        }
        else if(m_label_option == label_option_t::position)
        {
            const bool image_is_valid = medical_image::check_image_validity(image.get_shared());

            m_slice_selector_with_index->set_enabled(image_is_valid);

            const auto& origin = image->origin();
            m_axial_position = medical_image::get_slice_position(
                *image,
                axis_t::axial
            ).value_or(double(origin[axis_t::axial]));

            m_frontal_position = medical_image::get_slice_position(
                *image,
                axis_t::frontal
            ).value_or(double(origin[axis_t::frontal]));

            m_sagittal_position = medical_image::get_slice_position(
                *image,
                axis_t::sagittal
            ).value_or(double(origin[axis_t::sagittal]));

            this->update_slice_index_from_img(*image);
        }
    }

    this->update_slider_fiducial();
}

//----------------------------------------------------------------------------
void slice_index_position_editor::stopping()
{
    this->destroy();
    m_slice_selector_with_index = nullptr;
}

//------------------------------------------------------------------------------------

void slice_index_position_editor::destroyEditorContainer()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    qt_container->destroy_container();
}

//------------------------------------------------------
void slice_index_position_editor::update_slice_index(int _axial, int _frontal, int _sagittal)
{
    if(_sagittal != m_sagittal_index
       || _frontal != m_frontal_index
       || _axial != m_axial_index)
    {
        m_sagittal_index = _sagittal;
        m_frontal_index  = _frontal;
        m_axial_index    = _axial;

        const auto image = m_image.lock();

        medical_image::set_slice_index(*image, axis_t::axial, m_axial_index);
        medical_image::set_slice_index(*image, axis_t::frontal, m_frontal_index);
        medical_image::set_slice_index(*image, axis_t::sagittal, m_sagittal_index);

        const auto& origin = image->origin();
        m_sagittal_position = medical_image::get_slice_position(*image, axis_t::sagittal).value_or(origin[sagittal]);
        m_frontal_position  = medical_image::get_slice_position(*image, axis_t::frontal).value_or(origin[frontal]);
        m_axial_position    = medical_image::get_slice_position(*image, axis_t::axial).value_or(origin[axial]);

        this->update_slice_index_from_img(*image);
    }
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index_from_img(const sight::data::image& _image)
{
    if(medical_image::check_image_validity(_image))
    {
        if(m_label_option == label_option_t::index)
        {
            const auto image_size = _image.size();
            const auto index_ind  =
                medical_image::get_slice_index(_image, m_axis).value_or(image_size[m_axis] / 2);

            // Update QSlider
            int max = 0;
            if(_image.num_dimensions() > m_axis)
            {
                max = static_cast<int>(image_size[m_axis] - 1);
            }

            m_slice_selector_with_index->set_slice_range(0, max);
            m_slice_selector_with_index->set_slice_value(static_cast<int>(index_ind));

            // Find the max value for each dimension
            const int absolute_max = int(*std::ranges::max_element(image_size) - 1);
            m_slice_selector_with_index->set_index_digits(std::uint8_t(std::to_string(absolute_max).length()));
        }

        if(m_label_option == label_option_t::position)
        {
            const auto image_size = _image.size();

            const auto& spacing = _image.spacing();
            const auto& origin  = _image.origin();

            double max_position = 0.0;

            if(_image.num_dimensions() > m_axis)
            {
                max_position = static_cast<int>(image_size[m_axis] - 1);
            }

            double min_position = 0.00;

            m_slice_selector_with_index->set_position_range(min_position, max_position);

            const auto index_position =
                medical_image::get_slice_index(_image, m_axis).value_or(image_size[m_axis] / 2);

            m_slice_selector_with_index->set_image_info(origin[m_axis], spacing[m_axis]);
            m_slice_selector_with_index->set_position_text(static_cast<double>(index_position));
            m_slice_selector_with_index->set_position_value(static_cast<int>(index_position));
        }
    }
}

//------------------------------------------------------------------------------
void slice_index_position_editor::slice_index_notification(int _index)
{
    const auto image = m_image.lock();

    medical_image::set_slice_index(*image, m_axis, _index);

    std::array idx {
        static_cast<int>(medical_image::get_slice_index(*image, axis_t::sagittal).value_or(0)),
        static_cast<int>(medical_image::get_slice_index(*image, axis_t::frontal).value_or(0)),
        static_cast<int>(medical_image::get_slice_index(*image, axis_t::axial).value_or(0))
    };

    image->async_emit(this, data::image::SLICE_INDEX_MODIFIED_SIG, idx[2], idx[1], idx[0]);
}

//------------------------------------------------------------------------
void slice_index_position_editor::slice_label_notification()
{
    m_label_option = (m_label_option == label_option_t::index) ? label_option_t::position : label_option_t::index;

    this->destroyEditorContainer();
    this->create();
    auto* layout = new QVBoxLayout();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    qt_container->set_layout(layout);

    const QString service_id = QString::fromStdString(base_id());

    if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            static_cast<std::uint8_t>(1)

        );
    }
    else if(m_label_option == label_option_t::position)
    {
        m_slice_selector_with_index = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            0.0

        );
    }

    sight::ui::qt::slice_selector::ChangeIndexCallback fct_index = [this](int _t){slice_index_notification(_t);};
    sight::ui::qt::slice_selector::ChangeTypeCallback fct_type = [this](int _t){slice_type_notification(_t);};
    sight::ui::qt::slice_selector::ChangeLabelCallback fct_label = [this](){slice_label_notification();};

    m_slice_selector_with_index->setProperty("class", "slice_selector");
    m_slice_selector_with_index->set_enable(true);

    m_slice_selector_with_index->set_change_index_callback(fct_index);
    m_slice_selector_with_index->set_change_type_callback(fct_type);
    m_slice_selector_with_index->set_change_label_callback(fct_label);
    m_slice_selector_with_index->set_type_selection(m_axis);
    m_slice_selector_with_index->set_prefix(orientation_prefix_map.at(m_axis));

    m_slice_selector_with_index->setObjectName(service_id);

    layout->addWidget(m_slice_selector_with_index);

    layout->setContentsMargins(0, 0, 0, 0);

    this->updating();
}

//------------------------------------------------------------------------
void slice_index_position_editor::update_slice_type(int _from, int _to)
{
    if(_to == static_cast<int>(m_axis))
    {
        m_axis = static_cast<axis_t>(_from);
    }
    else if(_from == static_cast<int>(m_axis))
    {
        m_axis = static_cast<axis_t>(_to);
    }

    if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index->clear_slider_index();
    }
    else if(m_label_option == label_option_t::position)
    {
        m_slice_selector_with_index->clear_slider_position();
    }

    this->update_slice_type_from_img(m_axis);
}

//--------------------------------------------------------------------------

void slice_index_position_editor::update_slice_type_from_img(const axis_t& _type)
{
    if(m_label_option == label_option_t::position)
    {
        const std::string& new_orientation_prefix = orientation_prefix_map.at(_type);
        m_slice_selector_with_index->set_prefix(new_orientation_prefix);
        m_slice_selector_with_index->set_type_selection(static_cast<int>(_type));
    }
    else if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index->set_type_selection(static_cast<int>(_type));
    }

    const auto image = m_image.const_lock();
    SIGHT_ASSERT("The inout key '" + IMAGE_INOUT + "' is not defined.", image);
    this->update_slice_index_from_img(*image);
}

//------------------------------------------------------------------------------
void slice_index_position_editor::update_slider_fiducial()
{
    const auto image        = m_image.lock();
    const auto image_series = std::dynamic_pointer_cast<const sight::data::image_series>(image.get_shared());

    if(image_series)
    {
        if(m_label_option == label_option_t::index)
        {
            m_slice_selector_with_index->clear_slider_index();
        }
        else
        {
            m_slice_selector_with_index->clear_slider_position();
        }

        const auto fiducials_series = image_series->get_fiducials();
        const auto sets             = fiducials_series->get_fiducial_sets();

        std::vector<QColor> color_list;
        for(const auto& fiducials : sets)
        {
            std::array<float, 4> colors = {0.F, 0.F, 0.F, 1.F};

            if(fiducials.color.has_value())
            {
                colors = fiducials.color.value();
            }

            const QColor color = QColor::fromRgbF(
                colors[0],
                colors[1],
                colors[2],
                colors[3]
            );

            color_list.push_back(color);

            for(const auto& set : fiducials.fiducial_sequence)
            {
                for(const auto& point : set.contour_data)
                {
                    std::optional<std::int64_t> fiducial_position;
                    const std::array<double, 3> array_point = {point.x, point.y, point.z};

                    fiducial_position = geometry::data::get_fiducial_slice_index(*image, array_point, m_axis);

                    if(fiducial_position.has_value())
                    {
                        if(m_label_option == label_option_t::index)
                        {
                            m_slice_selector_with_index->add_slider_position(fiducial_position.value(), color);
                        }
                        else
                        {
                            m_slice_selector_with_index->add_position_slider(
                                static_cast<double>(fiducial_position.value()),
                                color
                            );
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void slice_index_position_editor::slice_type_notification(int _type)
{
    auto type = static_cast<axis_t>(_type);
    SIGHT_ASSERT(
        "Bad slice type " << type,
        type == axis_t::x_axis
        || type == axis_t::y_axis
        || type == axis_t::z_axis
    );

    const auto old_type = m_axis;

    // Change slice type
    m_axis = type;

    // Fire the signal
    {
        const auto image = m_image.const_lock();
        image->async_emit(this, data::image::SLICE_TYPE_MODIFIED_SIG, static_cast<int>(old_type), _type);

        this->update_slice_index_from_img(*image);
    }
}

//------------------------------------------------------------------------------
service::connections_t slice_index_position_editor::auto_connections() const
{
    connections_t connections;

    connections.push(IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT);
    connections.push(IMAGE_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT);

    connections.push(IMAGE_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INOUT, data::image::RULER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INOUT, data::image::FIDUCIAL_REMOVED_SIG, service::slots::UPDATE);

    const auto image        = m_image.lock();
    const auto image_series = std::dynamic_pointer_cast<const sight::data::image_series>(image.get_shared());

    if(image_series)
    {
        // connect fiducials added/removed to update slot.
        connections.push(IMAGE_INOUT, data::has_fiducials::signals::GROUP_REMOVED, service::slots::UPDATE);
        connections.push(IMAGE_INOUT, data::has_fiducials::signals::POINT_REMOVED, service::slots::UPDATE);
        connections.push(IMAGE_INOUT, data::has_fiducials::signals::POINT_ADDED, service::slots::UPDATE);
        connections.push(IMAGE_INOUT, data::has_fiducials::signals::POINT_MODIFIED, service::slots::UPDATE);
    }

    return connections;
}

} // namespace sight::module::ui::qt::image
