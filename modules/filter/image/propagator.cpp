/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/image/propagator.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <filter/image/image_diff.hpp>

#include <service/macros.hpp>

#include <ui/history/ImageDiffCommand.hpp>

namespace sight::module::filter::image
{

static const core::com::signals::key_t DRAWN_SIG = "drawn";

static const core::com::slots::key_t DRAW_SLOT            = "draw";
static const core::com::slots::key_t SET_ORIENTATION_SLOT = "setOrientation";
static const core::com::slots::key_t RESET_DRAWING        = "resetDrawing";

//-----------------------------------------------------------------------------

propagator::propagator() :
    has_parameters(m_slots)
{
    new_slot(DRAW_SLOT, &propagator::draw, this);
    new_slot(SET_ORIENTATION_SLOT, &propagator::setOrientation, this);
    new_slot(RESET_DRAWING, &propagator::resetDrawing, this);

    m_sigDrawn = new_signal<DrawnSignalType>(DRAWN_SIG);
}

//-----------------------------------------------------------------------------

propagator::~propagator()
= default;

//-----------------------------------------------------------------------------

void propagator::configuring()
{
    service::config_t config = this->get_config();

    m_value     = config.get<int>("value", 1);
    m_overwrite = config.get<bool>("overwrite", true);
    m_radius    = config.get<double>("radius", std::numeric_limits<double>::infinity());

    const std::string mode = config.get<std::string>("mode", "min");

    if(mode == "min")
    {
        m_mode = sight::filter::image::min_max_propagation::MIN;
    }
    else if(mode == "max")
    {
        m_mode = sight::filter::image::min_max_propagation::MAX;
    }
    else if(mode == "minmax")
    {
        m_mode = sight::filter::image::min_max_propagation::MINMAX;
    }
    else
    {
        SIGHT_FATAL("Unknown mode '" + mode + "'. Accepted values are 'min', 'max' or 'minmax'.");
    }

    const std::string orientation = config.get<std::string>("orientation", "axial");

    if(orientation == "sagital")
    {
        m_orientation = data::helper::MedicalImage::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = data::helper::MedicalImage::Y_AXIS;
    }
    else if(orientation == "axial")
    {
        m_orientation = data::helper::MedicalImage::Z_AXIS;
    }
    else
    {
        SIGHT_FATAL("Unknown orientation '" + orientation + "'");
    }
}

//-----------------------------------------------------------------------------

void propagator::starting()
{
    const auto imgInLock = m_imageIn.lock();
    SIGHT_ASSERT("No " << s_IMAGE_IN << " found.", imgInLock);
    const auto imgOutLock = m_imageOut.lock();
    SIGHT_ASSERT("No " << s_IMAGE_INOUT << " found.", imgOutLock);

    bool isValid = data::helper::MedicalImage::checkImageValidity(imgInLock.get_shared())
                   && data::helper::MedicalImage::checkImageValidity(imgOutLock.get_shared());

    SIGHT_FATAL_IF("Input and output image must have the same size.", imgInLock->size() != imgOutLock->size());
    SIGHT_WARN_IF(
        "Input and output image must have the same spacing.",
        imgInLock->getSpacing() != imgOutLock->getSpacing()
    );

    if(isValid)
    {
        m_propagator = std::make_unique<sight::filter::image::min_max_propagation>(
            imgInLock.get_shared(),
            imgOutLock.get_shared(),
            nullptr
        );
        m_lineDrawer = std::make_unique<sight::filter::image::line_drawer>(imgOutLock.get_shared(), nullptr);
    }
}

//-----------------------------------------------------------------------------

void propagator::updating()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void propagator::stopping()
{
    m_propagator.reset();
    m_drawing = false;
}

//-----------------------------------------------------------------------------

void propagator::setOrientation(int from, int to)
{
    if(to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<OrientationType>(from);
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<OrientationType>(to);
    }
}

//-----------------------------------------------------------------------------

void propagator::setBoolParameter(bool val, std::string key)
{
    SIGHT_WARN_IF("Key must be 'overwrite' for this slot to have an effect.", key != "overwrite");
    if(key == "overwrite")
    {
        m_overwrite = val;
    }
}

//-----------------------------------------------------------------------------

void propagator::setIntParameter(int val, std::string key)
{
    SIGHT_WARN_IF("Key must be 'value' for this slot to have an effect.", key != "value");
    if(key == "value")
    {
        m_value = val;
    }
}

//-----------------------------------------------------------------------------

void propagator::setDoubleParameter(double val, std::string key)
{
    SIGHT_WARN_IF("Key must be 'radius' for this slot to have an effect.", key != "radius");
    if(key == "radius")
    {
        m_radius = val;
    }
}

//-----------------------------------------------------------------------------

void propagator::setEnumParameter(std::string val, std::string key)
{
    SIGHT_WARN_IF("Key must be 'mode' for this slot to have an effect.", key != "mode");
    if(key == "mode")
    {
        if(val == "min")
        {
            m_mode = sight::filter::image::min_max_propagation::MIN;
        }
        else if(val == "max")
        {
            m_mode = sight::filter::image::min_max_propagation::MAX;
        }
        else if(val == "minmax")
        {
            m_mode = sight::filter::image::min_max_propagation::MINMAX;
        }
        else
        {
            SIGHT_WARN("Unknown mode '" + val + "'. Accepted values are 'min', 'max' or 'minmax'.");
        }
    }
}

//-----------------------------------------------------------------------------

void propagator::resetDrawing()
{
    m_drawing = false;
    m_diff.clear();
}

//-----------------------------------------------------------------------------

void propagator::draw(data::tools::picking_info pickingInfo)
{
    SIGHT_ASSERT("Drawer not instantiated, have you started the service ?", m_lineDrawer);

    const auto imgOutLock = m_imageOut.lock();
    SIGHT_ASSERT(s_IMAGE_INOUT << " does not exist", imgOutLock);

    SPTR(data::image::BufferType) val =
        data::helper::MedicalImage::getPixelInImageSpace(imgOutLock.get_shared(), m_value);

    const data::image::Spacing imgSpacing = imgOutLock->getSpacing();
    // Draw lines as thick as a single voxel.
    const double thickness = *std::min_element(imgSpacing.begin(), imgSpacing.end());

    CoordinatesType newPoint = {{static_cast<CoordinatesType::value_type>(pickingInfo.m_worldPos[0]),
        static_cast<CoordinatesType::value_type>(pickingInfo.m_worldPos[1]),
        static_cast<CoordinatesType::value_type>(pickingInfo.m_worldPos[2])
    }
    };

    bool imgBufferModified = false;
    if(pickingInfo.m_eventId == data::tools::picking_info::Event::MOUSE_LEFT_DOWN)
    {
        m_drawing  = true;
        m_oldPoint = newPoint;

        m_diff = m_lineDrawer->draw(m_orientation, m_oldPoint, m_oldPoint, val.get(), thickness, m_overwrite);

        imgBufferModified = m_diff.numElements() > 0;
    }
    else if(m_drawing && pickingInfo.m_eventId == data::tools::picking_info::Event::MOUSE_MOVE)
    {
        const auto diff = m_lineDrawer->draw(
            m_orientation,
            m_oldPoint,
            newPoint,
            val.get(),
            thickness,
            m_overwrite
        );
        m_oldPoint = newPoint;

        imgBufferModified = this->appendDiff(diff);
    }
    else if(m_drawing && pickingInfo.m_eventId == data::tools::picking_info::Event::MOUSE_LEFT_UP)
    {
        const auto diff = m_lineDrawer->draw(
            m_orientation,
            m_oldPoint,
            newPoint,
            val.get(),
            thickness,
            m_overwrite
        );

        imgBufferModified = this->appendDiff(diff);

        auto seeds = this->convertDiffToSeeds();

        sight::filter::image::image_diff propagDiff;

        propagDiff = m_propagator->propagate(seeds, val.get(), m_radius, m_overwrite, m_mode);

        imgBufferModified |= this->appendDiff(propagDiff);

        if(m_diff.numElements() > 0)
        {
            ui::history::ImageDiffCommand::sptr diffCommand(new ui::history::ImageDiffCommand(
                                                                imgOutLock.get_shared(),
                                                                m_diff
            ));
            m_sigDrawn->async_emit(diffCommand);
            m_diff.clear();
        }

        m_drawing = false;

        m_sigComputed->async_emit();
    }

    if(imgBufferModified)
    {
        auto sig = imgOutLock->signal<data::image::BufferModifiedSignalType>(data::image::BUFFER_MODIFIED_SIG);
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

bool propagator::appendDiff(const sight::filter::image::image_diff& diff)
{
    const bool append = (diff.numElements() > 0);

    if(append)
    {
        m_diff.addDiff(diff);
    }

    return append;
}

//-----------------------------------------------------------------------------

sight::filter::image::min_max_propagation::SeedsType propagator::convertDiffToSeeds() const
{
    const auto imgOut = m_imageOut.lock();
    SIGHT_ASSERT(s_IMAGE_INOUT << " does not exist", imgOut);

    const data::image::Size& imgSize = imgOut->size();

    sight::filter::image::min_max_propagation::SeedsType seeds;

    const std::size_t nbElts = m_diff.numElements();
    for(std::size_t i = 0 ; i < nbElts ; ++i)
    {
        data::image::IndexType index = m_diff.getElementDiffIndex(i);
        sight::filter::image::min_max_propagation::CoordinatesType coords;
        coords[0] = index % imgSize[0];
        coords[1] = (index / imgSize[0]) % imgSize[1];
        coords[2] = (index / imgSize[0]) / imgSize[1];

        seeds.push_back(coords);
    }

    return seeds;
}

//-----------------------------------------------------------------------------

service::connections_t propagator::auto_connections() const
{
    return {
        {s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, SET_ORIENTATION_SLOT},
        {s_IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, RESET_DRAWING}
    };
}

} // namespace sight::module::filter::image.
