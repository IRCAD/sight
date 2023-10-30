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

#include "modules/ui/qt/image/slice_index_position_editor.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>

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

const service::base::key_t slice_index_position_editor::IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

slice_index_position_editor::slice_index_position_editor() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &slice_index_position_editor::update_slice_index, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &slice_index_position_editor::update_slice_type, this);
}

//------------------------------------------------------------------------------

slice_index_position_editor::~slice_index_position_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_position_editor::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto* layout = new QVBoxLayout();

    m_slice_selector_panel = new sight::ui::qt::slice_selector(m_display_axis_selector, m_display_step_buttons);
    m_slice_selector_panel->setProperty("class", "slice_selector");
    m_slice_selector_panel->set_enable(false);
    m_slice_selector_panel->setObjectName(service_id);

    sight::ui::qt::slice_selector::ChangeIndexCallback change_index_callback;
    change_index_callback = [this](int _i){slice_index_notification(_i);};
    m_slice_selector_panel->set_change_index_callback(change_index_callback);

    sight::ui::qt::slice_selector::ChangeIndexCallback change_type_callback;
    change_type_callback = [this](int _t){slice_type_notification(_t);};
    m_slice_selector_panel->set_change_type_callback(change_type_callback);
    m_slice_selector_panel->set_type_selection(m_orientation);

    layout->addWidget(m_slice_selector_panel);
    layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void slice_index_position_editor::stopping()
{
    this->destroy();
    m_slice_selector_panel = nullptr;
}

//------------------------------------------------------------------------------

void slice_index_position_editor::configuring()
{
    this->initialize();

    const auto config = this->get_config();

    auto orientation = config.get<std::string>("sliceIndex");

    boost::algorithm::trim(orientation);
    boost::algorithm::to_lower(orientation);

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
        SIGHT_FATAL("The value for the xml element \"sliceIndex\" can only be axial, frontal or sagittal.");
    }

    m_display_axis_selector = config.get<bool>("displayAxisSelector", m_display_axis_selector);

    m_display_step_buttons = config.get<bool>("displayStepButtons", m_display_step_buttons);
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updating()
{
    const auto image = m_image.const_lock();

    const bool image_is_valid = imHelper::check_image_validity(image.get_shared());
    m_slice_selector_panel->set_enable(image_is_valid);

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
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index(int _axial, int _frontal, int _sagittal)
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

//-----------------------------------------------------------------------------

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

    this->update_slice_type_from_img(m_orientation);
}

//------------------------------------------------------------------------------

void slice_index_position_editor::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index_from_img(const sight::data::image& _image)
{
    if(imHelper::check_image_validity(_image))
    {
        // Default value take the middle of the size.
        const auto image_size = _image.size();
        const auto index      =
            imHelper::get_slice_index(_image, m_orientation).value_or(image_size[m_orientation] / 2);

        // Update QSlider
        int max = 0;
        if(_image.num_dimensions() > m_orientation)
        {
            max = static_cast<int>(_image.size()[m_orientation] - 1);
        }

        m_slice_selector_panel->set_slice_range(0, max);
        m_slice_selector_panel->set_slice_value(static_cast<int>(index));
    }
}

//------------------------------------------------------------------------------

void slice_index_position_editor::update_slice_type_from_img(const orientation_t& _type)
{
    // Update Type Choice
    m_slice_selector_panel->set_type_selection(static_cast<int>(_type));

    const auto image = m_image.const_lock();
    SIGHT_ASSERT("The inout key '" + IMAGE_INOUT + "' is not defined.", image);

    this->update_slice_index_from_img(*image);
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
    m_orientation = static_cast<orientation_t>(type);

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

service::connections_t slice_index_position_editor::auto_connections() const
{
    connections_t connections;

    connections.push(IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT);
    connections.push(IMAGE_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT);
    connections.push(IMAGE_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
