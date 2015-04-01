/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/helper/ChessboardAnalyzer.hpp"
#include "videoCalibration/SChessBoardDetector.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Array.hpp>
#include <fwData/PointList.hpp>

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Array.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/range/combine.hpp>

#include <algorithm>

namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SChessBoardDetector, ::fwData::Composite);

const ::fwCom::Slots::SlotKeyType SChessBoardDetector::s_DETECTPTS_SLOT = "detectPoints";
// ----------------------------------------------------------------------------

SChessBoardDetector::SChessBoardDetector() throw () : m_width(0),
                                                      m_height(0)
{
    m_slotDetectPts = ::fwCom::newSlot(&SChessBoardDetector::detectPoints, this);
    ::fwCom::HasSlots::m_slots(s_DETECTPTS_SLOT, m_slotDetectPts);




    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// ----------------------------------------------------------------------------

SChessBoardDetector::~SChessBoardDetector() throw ()
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::configuring() throw (fwTools::Failed)
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

        SLM_ASSERT("Attribute 'calInfo' is missing.", elem->hasAttribute("calInfo"));
        std::string calInfo = elem->getAttributeValue("calInfo");
        SLM_ASSERT("Attribute 'calInfo' is empty", !calInfo.empty());

        m_calInfoKeys.push_back(calInfo);

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

void SChessBoardDetector::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::swapping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updating() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::detectPoints( ::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    std::string tlKey, calInfoKey;
    std::vector< ::fwData::Image::sptr > images;
    std::vector< ::fwData::PointList::sptr > pointLists;

    BOOST_FOREACH(::boost::tie(tlKey, calInfoKey), ::boost::combine(m_timeLineKeys, m_calInfoKeys))
    {
        ::fwData::Composite::IteratorType it = composite->find(tlKey);
        SLM_ASSERT("Missing '" + tlKey + "' key in the composite", it !=  composite->end());
        ::extData::FrameTL::sptr frameTL = ::extData::FrameTL::dynamicCast(it->second);


        it = composite->find(calInfoKey);
        SLM_ASSERT("Missing '" + calInfoKey + "' key in the composite", it !=  composite->end());
        ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::dynamicCast(it->second);

        ::fwData::Image::sptr im = this->createImage(frameTL, timestamp);
        if(!im)
        {
            OSLM_WARN("No frame found in timeline for timestanp '"<<timestamp<<"'.");
            return;
        }

        ::fwData::PointList::sptr chessBoardPoints =
            ::videoCalibration::helper::ChessboardAnalyzer::detectChessboard(im, m_width, m_height);

        if(chessBoardPoints)
        {
            images.push_back(im);
            pointLists.push_back(chessBoardPoints);

        }
        else
        {
            SLM_WARN("Something went wrong in the detection of chessboard ! ");
            return;
        }
    }

    size_t idx = 0;
    for(const auto& key : m_calInfoKeys)
    {
        ::fwData::Composite::IteratorType it = composite->find(key);
        SLM_ASSERT("Missing '" + calInfoKey + "' key in the composite", it !=  composite->end());
        ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::dynamicCast(it->second);

        calInfo->addRecord(images[idx], pointLists[idx]);
        // Notify
        ::arData::CalibrationInfo::AddedRecordSignalType::sptr sig;
        sig = calInfo->signal< ::arData::CalibrationInfo::AddedRecordSignalType >
                  (::arData::CalibrationInfo::s_ADDED_RECORD_SIG);

        sig->asyncEmit();

        ++idx;
    }
}

// ----------------------------------------------------------------------------

::fwData::Image::sptr SChessBoardDetector::createImage( ::extData::FrameTL::sptr tl,
                                                        ::fwCore::HiResClock::HiResClockType timestamp)
{
    const CSPTR(::extData::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);
    if(!buffer)
    {
        return ::fwData::Image::sptr();
    }

    ::fwData::Image::sptr image = ::fwData::Image::New();

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

    return image;
}

} //namespace videoCalibration

