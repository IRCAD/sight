/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/PointList.hpp>

#include <arData/Camera.hpp>
#include <arData/CalibrationInfo.hpp>

#include <arlcore/Camera.h>
#include <arlcore/PointsList.h>
#include <arlcore/PlaneSystem.h>
#include <arlcore/Calibration.h>

#include "videoCalibration/SIntrinsic.hpp"

fwServicesRegisterMacro(::videoCalibration::ICalibration, ::videoCalibration::SIntrinsic, ::arData::Camera);

namespace videoCalibration
{

// ----------------------------------------------------------------------------

SIntrinsic::SIntrinsic() noexcept :
    m_width(0),
    m_height(0)
{

}

// ----------------------------------------------------------------------------

SIntrinsic::~SIntrinsic() noexcept
{

}

//------------------------------------------------------------------------------

void SIntrinsic::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr cfg = m_configuration->findConfigurationElement("calibrationInfoID");
    SLM_ASSERT("Tag 'calibrationInfoID' not found.", cfg);

    m_calibrationInfoID = cfg->getValue();
    SLM_ASSERT("'calibrationInfoID' is empty.", !m_calibrationInfoID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SLM_ASSERT("Tag 'board' not found.", cfgBoard);

    SLM_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    std::string width = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !width.empty());
    m_width = ::boost::lexical_cast< unsigned int> (width);

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    std::string height = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !height.empty());
    m_height = ::boost::lexical_cast< unsigned int> (height);
}

// ----------------------------------------------------------------------------

void SIntrinsic::starting()
{
}

// ----------------------------------------------------------------------------

void SIntrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SIntrinsic::swapping()
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SIntrinsic::updating()
{
    ::arData::Camera::sptr cam = this->getObject< ::arData::Camera >();

    ::fwTools::Object::sptr obj             = ::fwTools::fwID::getObject(m_calibrationInfoID);
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::dynamicCast(obj);
    SLM_ASSERT("Object with id '" + m_calibrationInfoID + "' is not an 'arData::CalibrationInfo'", calInfo);

    SLM_WARN_IF("Calibration info is empty.", calInfo->getPointListContainer().empty());
    if(!calInfo->getPointListContainer().empty())
    {
        ::arlCore::PlaneSystem planeSystem;

        std::vector< ::arlCore::PointList::csptr > models;
        std::vector< ::arlCore::PointList::csptr > points;
        ::arlCore::Camera arlCamera(planeSystem);
        std::vector< ::arlCore::vnl_rigid_matrix > ext;
        std::vector< double > opti = {1, 0, 1e-6, 1e-8, 1e-5};
        std::vector< double > log;

        ::arlCore::PointList::sptr refModel = ::arlCore::PointList::sptr(new ::arlCore::PointList);

        for (unsigned int y = 0; y < m_height - 1; ++y)
        {
            for (unsigned int x = 0; x < m_width - 1; ++x)
            {
                refModel->push_back(::arlCore::Point::PointFactory(x, y, 0));
            }
        }

        {
            ::fwData::mt::ObjectReadLock calInfoLock(calInfo);
            for(::fwData::PointList::sptr capture : calInfo->getPointListContainer())
            {
                ::arlCore::PointList::sptr dst = ::arlCore::PointList::sptr(new ::arlCore::PointList);

                for(::fwData::Point::csptr point : capture->getCRefPoints())
                {
                    SLM_ASSERT("point is null", point);
                    dst->push_back(::arlCore::Point::PointFactory(point->getCRefCoord()[0], point->getCRefCoord()[1]));
                }
                points.push_back(dst);
                models.push_back(refModel);
            }
        }

        ::arlCore::intrinsicCalibration(models, points, 8, arlCamera, ext, opti, log);

        ::fwData::mt::ObjectWriteLock camLock(cam);

        cam->setCx(arlCamera.getcx());
        cam->setCy(arlCamera.getcy());
        cam->setFx(arlCamera.getfx());
        cam->setFy(arlCamera.getfy());
        cam->setDistortionCoefficient(arlCamera.getkc(0), arlCamera.getkc(1), arlCamera.getkc(2), arlCamera.getkc(3),
                                      arlCamera.getkc(4));
        cam->setSkew(arlCamera.getAlphaC());

        ::fwData::Image::sptr img = calInfo->getImageContainer().front();

        cam->setWidth(static_cast<double>(img->getSize()[0]));
        cam->setHeight(static_cast<double>(img->getSize()[1]));

        cam->setIsCalibrated(true);

        ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
        sig = cam->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
            ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
