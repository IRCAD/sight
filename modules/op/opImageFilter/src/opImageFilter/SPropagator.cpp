/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "opImageFilter/SPropagator.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCommand/ImageDiffCommand.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/ImageDiff.hpp>

#include <fwServices/macros.hpp>

namespace opImageFilter
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opImageFilter::SPropagator, ::fwData::Image)

static const ::fwCom::Signals::SignalKeyType s_DRAWN_SIG = "drawn";

static const ::fwCom::Slots::SlotKeyType s_DRAW_SLOT            = "draw";
static const ::fwCom::Slots::SlotKeyType s_SET_ORIENTATION_SLOT = "setOrientation";
static const ::fwCom::Slots::SlotKeyType s_RESET_DRAWING        = "resetDrawing";

static const ::fwServices::IService::KeyType s_IMAGE_IN    = "imageIn";
static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "imageOut";

//-----------------------------------------------------------------------------

SPropagator::SPropagator() :
    m_value(1),
    m_overwrite(true),
    m_drawing(false)
{
    newSlot(s_DRAW_SLOT, &SPropagator::draw, this);
    newSlot(s_SET_ORIENTATION_SLOT, &SPropagator::setOrientation, this);
    newSlot(s_RESET_DRAWING, &SPropagator::resetDrawing, this);

    m_sigDrawn = newSignal< DrawnSignalType >(s_DRAWN_SIG);
}

//-----------------------------------------------------------------------------

SPropagator::~SPropagator()
{
}

//-----------------------------------------------------------------------------

void SPropagator::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_value     = config.get< int >("value", 1);
    m_overwrite = config.get< bool >("overwrite", true);
    m_radius    = config.get< double >("radius", std::numeric_limits<double>::infinity());

    const std::string mode = config.get< std::string >("mode", "min");

    if(mode == "min")
    {
        m_mode = ::imageFilterOp::MinMaxPropagation::MIN;
    }
    else if(mode == "max")
    {
        m_mode = ::imageFilterOp::MinMaxPropagation::MAX;
    }
    else if(mode == "minmax")
    {
        m_mode = ::imageFilterOp::MinMaxPropagation::MINMAX;
    }
    else
    {
        SLM_FATAL("Unknown mode '" + mode + "'. Accepted values are 'min', 'max' or 'minmax'.");
    }

    const std::string orientation = config.get< std::string >("orientation", "axial");

    if(orientation == "sagital")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Y_AXIS;
    }
    else if(orientation == "axial")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Z_AXIS;
    }
    else
    {
        SLM_FATAL("Unknown orientation '" + orientation + "'");
    }
}

//-----------------------------------------------------------------------------

void SPropagator::starting()
{
    const auto imgInLock  = this->getLockedInput< ::fwData::Image >(s_IMAGE_IN);
    const auto imgOutLock = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);
    ::fwData::Image::csptr imgIn = imgInLock.get_shared();
    ::fwData::Image::sptr imgOut = imgOutLock.get_shared();

    SLM_ASSERT("'imageIn' does not exist", imgIn);
    SLM_ASSERT("'imageOut' does not exist", imgOut);

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(imgIn) &&
                   ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(imgOut);

    SLM_FATAL_IF("Input and output image must have the same size.", imgIn->getSize2() != imgOut->getSize2());
    SLM_WARN_IF("Input and output image must have the same spacing.", imgIn->getSpacing2() != imgOut->getSpacing2());

    if (isValid)
    {
        m_propagator =
            UPTR(::imageFilterOp::MinMaxPropagation)(new ::imageFilterOp::MinMaxPropagation(imgIn, imgOut, nullptr));
        m_lineDrawer = UPTR(::dataOp::LineDrawer)(new ::dataOp::LineDrawer(imgOut, nullptr));
    }
}

//-----------------------------------------------------------------------------

void SPropagator::updating()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SPropagator::stopping()
{
    m_propagator.reset();
    m_drawing = false;
}

//-----------------------------------------------------------------------------

void SPropagator::setOrientation(int from, int to)
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

void SPropagator::setBoolParameter(bool val, std::string key)
{
    SLM_WARN_IF("Key must be 'overwrite' for this slot to have an effect.", key != "overwrite");
    if(key == "overwrite")
    {
        m_overwrite = val;
    }
}

//-----------------------------------------------------------------------------

void SPropagator::setIntParameter(int val, std::string key)
{
    SLM_WARN_IF("Key must be 'value' for this slot to have an effect.", key != "value");
    if(key == "value")
    {
        m_value = val;
    }
}

//-----------------------------------------------------------------------------

void SPropagator::setDoubleParameter(double val, std::string key)
{
    SLM_WARN_IF("Key must be 'radius' for this slot to have an effect.", key != "radius");
    if(key == "radius")
    {
        m_radius = val;
    }
}

//-----------------------------------------------------------------------------

void SPropagator::setEnumParameter(std::string val, std::string key)
{
    SLM_WARN_IF("Key must be 'mode' for this slot to have an effect.", key != "mode");
    if(key == "mode")
    {
        if(val == "min")
        {
            m_mode = ::imageFilterOp::MinMaxPropagation::MIN;
        }
        else if (val == "max")
        {
            m_mode = ::imageFilterOp::MinMaxPropagation::MAX;
        }
        else if(val == "minmax")
        {
            m_mode = ::imageFilterOp::MinMaxPropagation::MINMAX;
        }
        else
        {
            SLM_WARN("Unknown mode '" + val + "'. Accepted values are 'min', 'max' or 'minmax'.");
        }
    }
}

//-----------------------------------------------------------------------------

void SPropagator::resetDrawing()
{
    m_drawing = false;
    m_diff.clear();
}

//-----------------------------------------------------------------------------

void SPropagator::draw(::fwDataTools::PickingInfo pickingInfo)
{
    SLM_ASSERT("Drawer not instantiated, have you started the service ?", m_lineDrawer);

    const auto imgInLock  = this->getLockedInput< ::fwData::Image >(s_IMAGE_IN);
    const auto imgOutLock = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);
    ::fwData::Image::sptr image = imgOutLock.get_shared();

    SLM_ASSERT("'image' does not exist", image);

    SPTR(::fwData::Image::BufferType) val =
        ::fwDataTools::fieldHelper::MedicalImageHelpers::getPixelBufferInImageSpace(image, m_value);

    const ::fwData::Image::Spacing imgSpacing = image->getSpacing2();
    // Draw lines as thick as a single voxel.
    const double thickness = *std::min_element(imgSpacing.begin(), imgSpacing.end());

    CoordinatesType newPoint = {{ static_cast< CoordinatesType::value_type >(pickingInfo.m_worldPos[0]),
        static_cast< CoordinatesType::value_type >(pickingInfo.m_worldPos[1]),
        static_cast< CoordinatesType::value_type >(pickingInfo.m_worldPos[2]) }};

    bool imgBufferModified = false;
    if(pickingInfo.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN)
    {
        m_drawing  = true;
        m_oldPoint = newPoint;

        m_diff = m_lineDrawer->draw(m_orientation, m_oldPoint, m_oldPoint, val.get(), thickness, m_overwrite);

        imgBufferModified = m_diff.getNumberOfElements() > 0;
    }
    else if(m_drawing && pickingInfo.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE)
    {
        const ::fwDataTools::ImageDiff diff = m_lineDrawer->draw(m_orientation, m_oldPoint, newPoint,
                                                                 val.get(), thickness, m_overwrite);
        m_oldPoint = newPoint;

        imgBufferModified = this->appendDiff(diff);
    }
    else if(m_drawing && pickingInfo.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP)
    {
        const ::fwDataTools::ImageDiff diff = m_lineDrawer->draw(m_orientation, m_oldPoint, newPoint,
                                                                 val.get(), thickness, m_overwrite);

        imgBufferModified = this->appendDiff(diff);

        auto seeds = this->convertDiffToSeeds();

        ::fwDataTools::ImageDiff propagDiff;

        propagDiff = m_propagator->propagate(seeds, val.get(), m_radius, m_overwrite, m_mode);

        imgBufferModified |= this->appendDiff(propagDiff);

        if(m_diff.getNumberOfElements() > 0)
        {
            ::fwCommand::ImageDiffCommand::sptr diffCommand(new ::fwCommand::ImageDiffCommand(image, m_diff));
            m_sigDrawn->asyncEmit(diffCommand);
            m_diff.clear();
        }

        m_drawing = false;

        m_sigComputed->asyncEmit();
    }

    if(imgBufferModified)
    {
        auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

bool SPropagator::appendDiff(const fwDataTools::ImageDiff& diff)
{
    const bool append = (diff.getNumberOfElements() > 0);

    if(append)
    {
        m_diff.addDiff(diff);
    }

    return append;
}

//-----------------------------------------------------------------------------

::imageFilterOp::MinMaxPropagation::SeedsType SPropagator::convertDiffToSeeds() const
{
    const auto imgOut = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);

    const ::fwData::Image::Size& imgSize = imgOut->getSize2();

    ::imageFilterOp::MinMaxPropagation::SeedsType seeds;

    const size_t nbElts = m_diff.getNumberOfElements();
    for(size_t i = 0; i < nbElts; ++i)
    {
        ::fwData::Image::IndexType index = m_diff.getElementDiffIndex(i);
        ::imageFilterOp::MinMaxPropagation::CoordinatesType coords;
        coords[0] = index % imgSize[0];
        coords[1] = (index / imgSize[0]) % imgSize[1];
        coords[2] = (index / imgSize[0]) / imgSize[1];

        seeds.push_back(coords);
    }

    return seeds;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPropagator::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SET_ORIENTATION_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_RESET_DRAWING);

    return connections;
}

} // namespace opImageFilter.
