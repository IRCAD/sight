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

#include "modules/ui/qt/image/image_info.hpp"

#include <core/base.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace sight::module::ui::qt::image
{

static const core::com::slots::key_t GET_INTERACTION_SLOT = "get_interaction";

image_info::image_info() noexcept
{
    new_slot(GET_INTERACTION_SLOT, &image_info::get_interaction, this);
}

//------------------------------------------------------------------------------

image_info::~image_info() noexcept =
    default;

//------------------------------------------------------------------------------

void image_info::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* h_layout = new QHBoxLayout();

    auto* static_text = new QLabel(QObject::tr("intensity:"));
    h_layout->addWidget(static_text, 0, Qt::AlignVCenter);

    m_value_text = new QLineEdit();
    m_value_text->setReadOnly(true);
    h_layout->addWidget(m_value_text, 1, Qt::AlignVCenter);

    qt_container->set_layout(h_layout);
}

//------------------------------------------------------------------------------

void image_info::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void image_info::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void image_info::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("The input '" << IMAGE << "' is not defined", image);
    const bool image_is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    m_value_text->setEnabled(image_is_valid);
}

//------------------------------------------------------------------------------

void image_info::get_interaction(data::tools::picking_info _info)
{
    if(_info.m_event_id == data::tools::picking_info::event::mouse_move)
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("The input '" << IMAGE << "' is not defined", image);

        const bool image_is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
        m_value_text->setEnabled(image_is_valid);
        if(image_is_valid)
        {
            const std::array<double, 3>& point = _info.m_world_pos;
            const data::image::size_t size     = image->size();

            if(point[0] < 0 || point[1] < 0 || point[2] < 0)
            {
                SIGHT_ERROR(
                    "The received coordinates are not in image space, maybe you used the wrong picker "
                    "interactor (see ::visuVTKAdaptor::imagePickerInteractor)"
                );
                return;
            }

            const data::image::size_t coords =
            {{static_cast<data::image::size_t::value_type>(point[0]),
                static_cast<data::image::size_t::value_type>(point[1]),
                static_cast<data::image::size_t::value_type>(point[2])
            }
            };

            bool is_inside = (coords[0] < size[0] && coords[1] < size[1]);
            if(image->num_dimensions() < 3)
            {
                is_inside = (is_inside && coords[2] == 0);
            }
            else
            {
                is_inside = (is_inside && coords[2] < size[2]);
            }

            if(!is_inside)
            {
                SIGHT_ERROR(
                    "The received coordinates are not in image space, maybe you used the wrong picker "
                    "interactor (see ::visuVTKAdaptor::imagePickerInteractor)"
                );
                return;
            }

            const auto dump_lock = image->dump_lock();

            const std::string intensity = image->get_pixel_as_string(coords[0], coords[1], coords[2]);
            m_value_text->setText(QString::fromStdString(intensity));
        }
    }
}

//------------------------------------------------------------------------------

void image_info::info(std::ostream& _sstream)
{
    _sstream << "image Info Editor";
}

//------------------------------------------------------------------------------

service::connections_t image_info::auto_connections() const
{
    connections_t connections;

    connections.push(IMAGE, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
