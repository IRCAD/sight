/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SCheckChessBoard.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Array.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Array.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include <arData/CalibrationInfo.hpp>
#include "videoCalibration/helper/ChessboardAnalyzer.hpp"

#include <algorithm>

#include <boost/tuple/tuple.hpp>
#include <boost/range/combine.hpp>

namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SCheckChessBoard, ::fwData::Composite);

const ::fwCom::Signals::SignalKeyType SCheckChessBoard::s_CHESSBOARD_DETECTED_SIG     = "chessboardDetected";
const ::fwCom::Signals::SignalKeyType SCheckChessBoard::s_CHESSBOARD_NOT_DETECTED_SIG = "chessboardNotDetected";

const ::fwCom::Slots::SlotKeyType SCheckChessBoard::s_DETECT_POINTS_SLOT = "detectPoints";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT   = "updateChessboardSize";
// ----------------------------------------------------------------------------

SCheckChessBoard::SCheckChessBoard() throw () : m_width(0),
                                                m_height(0)
{
    m_sigChessboardDetected    = ChessboardDetectedSignalType::New();
    m_sigChessboardNotDetected = ChessboardNotDetectedSignalType::New();
    m_slotDetectPoints         = ::fwCom::newSlot(&SCheckChessBoard::detectPoints, this);

    ::fwCom::HasSlots::m_slots(s_DETECT_POINTS_SLOT, m_slotDetectPoints);

    ::fwCom::HasSignals::m_signals(s_CHESSBOARD_DETECTED_SIG, m_sigChessboardDetected)
        (s_CHESSBOARD_NOT_DETECTED_SIG, m_sigChessboardNotDetected);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    m_slotUpdateChessboardSize = newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SCheckChessBoard::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SCheckChessBoard::~SCheckChessBoard() throw ()
{
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::configuring() throw (fwTools::Failed)
{
    ::fwRuntime::ConfigurationElementContainer caliCfgs = m_configuration->findAllConfigurationElement("calibration");
    SLM_ASSERT("Can not find calibration element", caliCfgs.size() >= 1 );
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    for(ConfigurationType elem :  caliCfgs.getElements())
    {
        SLM_ASSERT("Attribute 'timeline' is missing.", elem->hasAttribute("timeline"));
        std::string tl = elem->getAttributeValue("timeline");
        SLM_ASSERT("Attribute 'timeline' is empty", !tl.empty());

        m_timeLineKeys.push_back(tl);
    }

    ::fwRuntime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SLM_ASSERT("Tag 'board' not found.", cfgBoard);

    SLM_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    std::string width = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !width.empty());
    m_width = ::boost::lexical_cast< size_t > (width);

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    std::string height = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !height.empty());
    m_height = ::boost::lexical_cast< size_t > (height);
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::swapping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::updating() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::detectPoints( ::fwCore::HiResClock::HiResClockType timestamp)
{
    bool isDetected = true;

    if (timestamp > m_lastTimestamp)
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwCore::HiResClock::HiResClockType lastTimestamp;
        lastTimestamp = std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

        std::vector< ::extData::FrameTL::sptr > timelines;
        timelines.reserve(m_timeLineKeys.size());

        for(const std::string &tlKey : m_timeLineKeys)
        {
            ::fwData::Composite::IteratorType it = composite->find(tlKey);
            SLM_ASSERT("Missing '" + tlKey + "' key in the composite", it !=  composite->end());
            ::extData::FrameTL::sptr frameTL = ::extData::FrameTL::dynamicCast(it->second);

            timelines.push_back(frameTL);

            lastTimestamp = std::min(lastTimestamp, frameTL->getNewerTimestamp());
        }

        size_t idx = 0;
        for(const std::string &tlKey : m_timeLineKeys)
        {
            ::fwData::Image::sptr im = this->createImage(timelines[idx], lastTimestamp);
            if (!im)
            {
                isDetected = false;
                break;
            }

            ::fwData::PointList::sptr chessBoardPoints =
                ::videoCalibration::helper::ChessboardAnalyzer::detectChessboard(im, m_width, m_height);

            if(!chessBoardPoints)
            {
                isDetected = false;
                break;

            }
            ++idx;
        }

        if(isDetected)
        {
            m_sigChessboardDetected->asyncEmit();
        }
        else
        {
            m_sigChessboardNotDetected->asyncEmit();
        }

        m_lastTimestamp = lastTimestamp;
    }
}

// ----------------------------------------------------------------------------

::fwData::Image::sptr SCheckChessBoard::createImage( ::extData::FrameTL::sptr tl,
                                                     ::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwData::Image::sptr image;

    const CSPTR(::extData::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);
    if (buffer)
    {
        image = ::fwData::Image::New();

        ::fwData::Image::SizeType size(3);
        size[0] = tl->getWidth();
        size[1] = tl->getHeight();
        size[2] = 1;
        const ::fwData::Image::SpacingType::value_type voxelSize = 1;
        image->allocate(size, tl->getType(), tl->getNumberOfComponents());
        ::fwData::Image::OriginType origin(3,0);

        image->setOrigin(origin);
        ::fwData::Image::SpacingType spacing(3,voxelSize);
        image->setSpacing(spacing);
        image->setWindowWidth(1);
        image->setWindowCenter(0);

        ::fwData::Array::sptr array = image->getDataArray();

        ::fwComEd::helper::Array arrayHelper(array);

        const ::boost::uint8_t*  frameBuff = &buffer->getElement(0);
        ::boost::uint8_t* index = arrayHelper.begin< ::boost::uint8_t >();

        std::copy( frameBuff, frameBuff+buffer->getSize(), index);
    }

    return image;
}

// ----------------------------------------------------------------------------

void SCheckChessBoard::updateChessboardSize(const int width, const int height, const float squareSize)
{
    m_width  = width;
    m_height = height;
    m_sigChessboardNotDetected->asyncEmit();
}

} //namespace videoCalibration

