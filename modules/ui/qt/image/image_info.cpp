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

#include "modules/ui/qt/image/image_info.hpp"

#include <core/base.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <geometry/data/types.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace sight::module::ui::qt::image
{

static const core::com::slots::key_t GET_INTERACTION_SLOT = "getInteraction";

image_info::image_info() noexcept
{
    new_slot(GET_INTERACTION_SLOT, &image_info::getInteraction, this);
}

//------------------------------------------------------------------------------

image_info::~image_info() noexcept =
    default;

//------------------------------------------------------------------------------

void image_info::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* h_layout = new QHBoxLayout();

    auto* staticText = new QLabel(QObject::tr("intensity:"));
    h_layout->addWidget(staticText, 0, Qt::AlignVCenter);

    m_valueText = new QLineEdit();
    m_valueText->setReadOnly(true);
    h_layout->addWidget(m_valueText, 1, Qt::AlignVCenter);

    qtContainer->setLayout(h_layout);
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
    SIGHT_ASSERT("The input '" << s_IMAGE << "' is not defined", image);
    const bool imageIsValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    m_valueText->setEnabled(imageIsValid);
}

//------------------------------------------------------------------------------

void image_info::getInteraction(data::tools::picking_info info)
{
    if(info.m_eventId == data::tools::picking_info::Event::MOUSE_MOVE)
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("The input '" << s_IMAGE << "' is not defined", image);

        const bool imageIsValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
        m_valueText->setEnabled(imageIsValid);
        if(imageIsValid)
        {
            const std::array<double, 3>& point = info.m_worldPos;
            const data::image::Size size       = image->size();

            if(point[0] < 0 || point[1] < 0 || point[2] < 0)
            {
                SIGHT_ERROR(
                    "The received coordinates are not in image space, maybe you used the wrong picker "
                    "interactor (see ::visuVTKAdaptor::imagePickerInteractor)"
                );
                return;
            }

            const data::image::Size coords =
            {{static_cast<data::image::Size::value_type>(point[0]),
                static_cast<data::image::Size::value_type>(point[1]),
                static_cast<data::image::Size::value_type>(point[2])
            }
            };

            bool isInside = (coords[0] < size[0] && coords[1] < size[1]);
            if(image->numDimensions() < 3)
            {
                isInside = (isInside && coords[2] == 0);
            }
            else
            {
                isInside = (isInside && coords[2] < size[2]);
            }

            if(!isInside)
            {
                SIGHT_ERROR(
                    "The received coordinates are not in image space, maybe you used the wrong picker "
                    "interactor (see ::visuVTKAdaptor::imagePickerInteractor)"
                );
                return;
            }

            const auto dumpLock = image->dump_lock();

            const std::string intensity = image->getPixelAsString(coords[0], coords[1], coords[2]);
            m_valueText->setText(QString::fromStdString(intensity));
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

    connections.push(s_IMAGE, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
