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

#include "modules/ui/qt/image/slice_index_position_editor.hpp"

#include "data/image_series.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/spy_log.hpp>

#include <data/helper/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>

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

namespace imHelper = data::helper::medical_image;

static const core::com::slots::key_t UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

const service::base::key_t slice_index_position_editor::IMAGE_INOUT =
    "image";

std::map<data::helper::medical_image::orientation_t,
         std::string> slice_index_position_editor::orientation_prefix_map = {
    {data::helper::medical_image::orientation_t::axial, "S"},
    {data::helper::medical_image::orientation_t::frontal, "P"},
    {data::helper::medical_image::orientation_t::sagittal, "L"}
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
        m_orientation = orientation_t::axial; // Z
    }
    else if(orientation == "frontal")
    {
        m_orientation = orientation_t::frontal; // Y
    }
    else if(orientation == "sagittal")
    {
        m_orientation = orientation_t::sagittal; // X
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

    if(m_label_option == label_option_t::index)
    {
        sight::ui::qt::slice_selector::ChangeIndexCallback fct_index = [this](int _index)
                                                                       {
                                                                           slice_index_notification(_index);
                                                                       };
        sight::ui::qt::slice_selector::ChangeTypeCallback fct_type = [this](int _type){slice_type_notification(_type);};

        m_slice_selector_with_index = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            static_cast<std::uint8_t>(1)

        );

        m_slice_selector_with_index->setProperty("class", "slice_selector");
        m_slice_selector_with_index->set_enable(false);
        m_slice_selector_with_index->setObjectName(service_id);

        m_slice_selector_with_index->set_change_index_callback(fct_index);
        m_slice_selector_with_index->set_change_type_callback(fct_type);
        m_slice_selector_with_index->set_type_selection(m_orientation);

        layout->addWidget(m_slice_selector_with_index);

        layout->setContentsMargins(0, 0, 0, 0);

        qt_container->set_layout(layout);

        this->updating();
    }
    else if(m_label_option == label_option_t::position)
    {
        sight::ui::qt::slice_selector::ChangeIndexCallback fct_index = [this](int _t){slice_index_notification(_t);};
        sight::ui::qt::slice_selector::ChangeTypeCallback fct_type = [this](int _t){slice_type_notification(_t);};

        m_slice_selector_with_position = new sight::ui::qt::slice_selector(
            m_display_axis_selector,
            m_display_step_buttons,
            0.0

        );
        m_slice_selector_with_position->setProperty("class", "slice_selector");
        m_slice_selector_with_position->set_enabled(false);
        m_slice_selector_with_position->setObjectName(service_id);

        m_slice_selector_with_position->set_change_index_callback(fct_index);
        m_slice_selector_with_position->set_change_type_callback(fct_type);
        m_slice_selector_with_position->set_type_selection(m_orientation);

        layout->addWidget(m_slice_selector_with_position);

        m_slice_selector_with_position->set_prefix(orientation_prefix_map.at(m_orientation));

        layout->setContentsMargins(0, 0, 0, 0);

        qt_container->set_layout(layout);

        this->updating();
    }
    else
    {
        SIGHT_ERROR("not a known value of label .");
    }
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updating()
{
    {
        const auto image = m_image.const_lock();

        if(m_label_option == label_option_t::index)
        {
            const bool image_is_valid = imHelper::check_image_validity(image.get_shared());

            m_slice_selector_with_index->set_enable(image_is_valid);

            m_axial_index = std::max(
                std::int64_t(0),
                imHelper::get_slice_index(*image, imHelper::orientation_t::axial).value_or(0)
            );
            m_frontal_index = std::max(
                std::int64_t(0),
                imHelper::get_slice_index(*image, imHelper::orientation_t::frontal).value_or(0)
            );
            m_sagittal_index = std::max(
                std::int64_t(0),
                imHelper::get_slice_index(*image, imHelper::orientation_t::sagittal).value_or(0)
            );

            this->update_slice_index_from_img(*image);
            this->update_label(*image);
        }
        else if(m_label_option == label_option_t::position)
        {
            const bool image_is_valid = imHelper::check_image_validity(image.get_shared());

            m_slice_selector_with_position->set_enabled(image_is_valid);

            m_axial_position =
                static_cast<double>(imHelper::get_slice_position(*image, imHelper::orientation_t::axial)
                                    .value_or(image->origin()[2]));

            m_frontal_position =
                static_cast<double>(imHelper::get_slice_position(*image, imHelper::orientation_t::frontal)
                                    .value_or(image->origin()[1]));

            m_sagittal_position =
                static_cast<double>(imHelper::get_slice_position(*image, imHelper::orientation_t::sagittal)
                                    .value_or(image->origin()[0]));

            this->update_slice_index_from_img(*image);
            this->update_label(*image);
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

//-----------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index(int _axial, int _frontal, int _sagittal)
{
    if(m_label_option == label_option_t::index)
    {
        if(_sagittal != m_sagittal_index
           || _frontal != m_frontal_index
           || _axial != m_axial_index)
        {
            m_sagittal_index = _sagittal;
            m_frontal_index  = _frontal;
            m_axial_index    = _axial;
        }

        const auto image = m_image.lock();

        namespace imHelper = imHelper;

        imHelper::set_slice_index(*image, orientation_t::axial, m_axial_index);
        imHelper::set_slice_index(*image, orientation_t::frontal, m_frontal_index);
        imHelper::set_slice_index(*image, orientation_t::sagittal, m_sagittal_index);

        this->update_slice_index_from_img(*image);
    }
    else if(m_label_option == label_option_t::position)
    {
        const auto image = m_image.lock();

        namespace imHelper = imHelper;

        const auto& origin  = image->origin();
        const auto& spacing = image->spacing();

        double sagittal_p = origin[0] + _sagittal * spacing[0];
        double frontal_p  = origin[1] + _frontal * spacing[1];
        double axial_p    = origin[2] + _axial * spacing[2];

        if(_sagittal != m_sagittal_index
           || _frontal != m_frontal_index
           || _axial != m_axial_index)
        {
            m_sagittal_position = sagittal_p;
            m_frontal_position  = frontal_p;
            m_axial_position    = axial_p;
        }

        imHelper::set_slice_position(*image, orientation_t::axial, m_axial_position);
        imHelper::set_slice_position(*image, orientation_t::frontal, m_frontal_position);
        imHelper::set_slice_position(*image, orientation_t::sagittal, m_sagittal_position);

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
    if(imHelper::check_image_validity(_image))
    {
        if(m_label_option == label_option_t::index)
        {
            const auto image_size = _image.size();
            const auto index_ind  =
                imHelper::get_slice_index(_image, m_orientation).value_or(image_size[m_orientation] / 2);

            // Update QSlider
            int max = 0;
            if(_image.num_dimensions() > m_orientation)
            {
                max = static_cast<int>(image_size[m_orientation] - 1);
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

            if(_image.num_dimensions() > m_orientation)
            {
                max_position = static_cast<int>(image_size[m_orientation] - 1);
            }

            double min_position = 0.00;

            m_slice_selector_with_position->set_position_range(min_position, max_position);

            const auto index_position =
                imHelper::get_slice_index(_image, m_orientation).value_or(image_size[m_orientation] / 2);

            m_slice_selector_with_position->set_image_info(origin[m_orientation], spacing[m_orientation]);
            m_slice_selector_with_position->set_position_text(static_cast<double>(index_position));
            m_slice_selector_with_position->set_position_value(static_cast<int>(index_position));
        }
    }
}

//------------------------------------------------------------------------------
void slice_index_position_editor::slice_index_notification(int _index)
{
    const auto image = m_image.lock();

    imHelper::set_slice_index(*image, m_orientation, _index);

    std::array idx {
        static_cast<int>(imHelper::get_slice_index(
                             *image,
                             imHelper::orientation_t::sagittal
        ).value_or(0)),
        static_cast<int>(imHelper::get_slice_index(
                             *image,
                             imHelper::orientation_t::frontal
        ).value_or(0)),
        static_cast<int>(imHelper::get_slice_index(
                             *image,
                             imHelper::orientation_t::axial
        ).value_or(0))
    };

    auto sig = image->signal<data::image::slice_index_modified_signal_t>(
        data::image::SLICE_INDEX_MODIFIED_SIG
    );
    core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_INDEX_SLOT)));
    sig->async_emit(idx[2], idx[1], idx[0]);
}

//------------------------------------------------------------------------
void slice_index_position_editor::update_slice_type(int _from, int _to)
{
    if(_to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_to);
    }

    if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index->clear_slider_index();
    }
    else if(m_label_option == label_option_t::position)
    {
        m_slice_selector_with_position->clear_slider_position();
    }

    this->update_slice_type_from_img(m_orientation);
}

//--------------------------------------------------------------------------

void slice_index_position_editor::update_slice_type_from_img(const orientation_t& _type)
{
    if(m_label_option == label_option_t::position)
    {
        const std::string& new_orientation_prefix = orientation_prefix_map.at(_type);
        m_slice_selector_with_position->set_prefix(new_orientation_prefix);
        m_slice_selector_with_position->set_type_selection(static_cast<int>(_type));
        m_slice_selector_with_position->update_label();
    }
    else if(m_label_option == label_option_t::index)
    {
        m_slice_selector_with_index->set_type_selection(static_cast<int>(_type));
        m_slice_selector_with_index->update_label();
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
            m_slice_selector_with_position->clear_slider_position();
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

                    fiducial_position = imHelper::get_fiducial_slice_index(*image, array_point, m_orientation);

                    if(fiducial_position.has_value())
                    {
                        if(m_label_option == label_option_t::index)
                        {
                            m_slice_selector_with_index->add_slider_position(fiducial_position.value(), color);
                        }
                        else
                        {
                            m_slice_selector_with_position->add_position_slider(
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
    auto type = static_cast<orientation_t>(_type);
    SIGHT_ASSERT(
        "Bad slice type " << type,
        type == orientation_t::x_axis
        || type == orientation_t::y_axis
        || type == orientation_t::z_axis
    );

    const auto old_type = m_orientation;

    // Change slice type
    m_orientation = type;

    // Fire the signal
    {
        const auto image = m_image.const_lock();

        auto sig = image->signal<data::image::slice_type_modified_signal_t>(
            data::image::SLICE_TYPE_MODIFIED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_TYPE_SLOT)));
            sig->async_emit(old_type, _type);
        }
        this->update_slice_index_from_img(*image);
    }
}

//------------------------------------------------------------------------------
void slice_index_position_editor::update_label(const sight::data::image& _image)
{
    if(m_label_option == label_option_t::position)
    {
        if(m_slice_selector_with_position != nullptr)
        {
            m_slice_selector_with_position->update_label();
        }
    }
    else if(m_label_option == label_option_t::index)
    {
        if(m_slice_selector_with_index != nullptr)
        {
            const auto& spacing = _image.spacing();
            const auto& origin  = _image.origin();

            m_slice_selector_with_index->set_image_info(origin[m_orientation], spacing[m_orientation]);

            const std::string& new_orientation_prefix = orientation_prefix_map.at(m_orientation);
            m_slice_selector_with_index->set_prefix(new_orientation_prefix);

            m_slice_selector_with_index->update_label();
        }
    }

    this->update_slice_index_from_img(_image);
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
