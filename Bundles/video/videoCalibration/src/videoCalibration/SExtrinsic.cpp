/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SExtrinsic.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <arlcore/Calibration.h>
#include <arlcore/Camera.h>
#include <arlcore/PlaneSystem.h>
#include <arlcore/PointsList.h>

fwServicesRegisterMacro(::videoCalibration::ICalibration, ::videoCalibration::SExtrinsic, ::arData::CameraSeries);

namespace videoCalibration
{

// ----------------------------------------------------------------------------

SExtrinsic::SExtrinsic() noexcept :
    m_width(0),
    m_height(0),
    m_camIndex(1)
{

}

// ----------------------------------------------------------------------------

SExtrinsic::~SExtrinsic() noexcept
{

}

//------------------------------------------------------------------------------

void SExtrinsic::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr cfg1 = m_configuration->findConfigurationElement("calibrationInfo1ID");
    SLM_ASSERT("Tag 'calibrationInfo1ID' not found.", cfg1);

    m_calibrationInfo1ID = cfg1->getValue();
    SLM_ASSERT("'calibrationInfo1ID' is empty.", !m_calibrationInfo1ID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfg2 = m_configuration->findConfigurationElement("calibrationInfo2ID");
    SLM_ASSERT("Tag 'calibrationInfo2ID' not found.", cfg2);

    m_calibrationInfo2ID = cfg2->getValue();
    SLM_ASSERT("'calibrationInfo2ID' is empty.", !m_calibrationInfo2ID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfgIdx = m_configuration->findConfigurationElement("camIndex");
    if(cfgIdx)
    {
        std::string idxStr = cfgIdx->getValue();
        SLM_ASSERT("'camIndex' is empty.", !idxStr.empty());
        m_camIndex = ::boost::lexical_cast<size_t>(idxStr);
    }

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

void SExtrinsic::starting()
{
}

// ----------------------------------------------------------------------------

void SExtrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SExtrinsic::swapping()
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SExtrinsic::updating()
{
    ::arData::CameraSeries::sptr camSeries = this->getObject< ::arData::CameraSeries >();

    ::fwTools::Object::sptr obj1             = ::fwTools::fwID::getObject(m_calibrationInfo1ID);
    ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(obj1);
    SLM_ASSERT("Object with id '" + m_calibrationInfo1ID + "' is not an 'arData::CalibrationInfo'", calInfo1);

    ::fwTools::Object::sptr obj2             = ::fwTools::fwID::getObject(m_calibrationInfo2ID);
    ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::dynamicCast(obj2);
    SLM_ASSERT("Object with id '" + m_calibrationInfo2ID + "' is not an 'arData::CalibrationInfo'", calInfo2);

    SLM_ASSERT("camera index must be > 0 and < camSeries->getNumberOfCameras()",
               m_camIndex > 0 && m_camIndex < camSeries->getNumberOfCameras());

    SLM_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!calInfo1->getPointListContainer().empty())
    {
        ::arlCore::PlaneSystem planeSystem;

        std::vector< ::arlCore::PointList::csptr > models;
        std::vector< std::vector< std::vector< ::arlCore::Point::csptr > > > points;
        ::arlCore::Camera arlCamera1(planeSystem);
        ::arlCore::Camera arlCamera2(planeSystem);
        std::vector< const ::arlCore::Camera* > cameras = {&arlCamera1, &arlCamera2};
        std::vector< ::arlCore::vnl_rigid_matrix > ext_matrices;
        std::vector< double > opti = {0, 0, 1e-2, 1e-8, 1e-5};
        std::vector< double > log;

        // Set the model
        ::arlCore::PointList::sptr refModel = ::arlCore::PointList::sptr(new ::arlCore::PointList);

        for (unsigned int y = 0; y < m_width - 1; ++y)
        {
            for (unsigned int x = 0; x < m_height - 1; ++x)
            {
                refModel->push_back(::arlCore::Point::PointFactory(x * 20, y * 20, 0));
            }
        }

        {
            ::fwData::mt::ObjectReadLock calInfo1Lock(calInfo1);
            ::fwData::mt::ObjectReadLock calInfo2Lock(calInfo2);

            ::arData::CalibrationInfo::PointListContainerType ptlists1 = calInfo1->getPointListContainer();
            ::arData::CalibrationInfo::PointListContainerType ptlists2 = calInfo2->getPointListContainer();

            SLM_ASSERT("The two calibrationInfo have not the same size", ptlists1.size() == ptlists2.size());

            ::arData::CalibrationInfo::PointListContainerType::iterator itr1   = ptlists1.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr2   = ptlists2.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itrEnd = ptlists1.end();

            for(; itr1 != itrEnd; ++itr1, ++itr2)
            {
                ::fwData::PointList::sptr ptList1 = *itr1;
                ::fwData::PointList::sptr ptList2 = *itr2;

                std::vector< std::vector< ::arlCore::Point::csptr > > acq(2);

                for(fwData::Point::csptr point : ptList1->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    acq[0].push_back(
                        ::arlCore::Point::PointFactory(point->getCoord()[0], point->getCoord()[1]));
                }

                for(fwData::Point::csptr point : ptList2->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    acq[1].push_back(
                        ::arlCore::Point::PointFactory(point->getCoord()[0], point->getCoord()[1]));
                }

                points.push_back(acq);
                models.push_back(refModel);
            }
        }

        // Set the cameras
        vnl_vector< double > intrinsicParam(10);

        {
            ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);
            ::arData::Camera::sptr cam1 = camSeries->getCamera(0);
            ::arData::Camera::sptr cam2 = camSeries->getCamera(m_camIndex);

            ::fwData::mt::ObjectReadLock cam1Lock(cam1);
            ::fwData::mt::ObjectReadLock cam2Lock(cam2);

            intrinsicParam[0] = cam1->getFx();
            intrinsicParam[1] = cam1->getFy();
            intrinsicParam[2] = cam1->getCx();
            intrinsicParam[3] = cam1->getCy();
            for (int i = 0; i < 5; ++i)
            {
                intrinsicParam[i + 4] = cam1->getDistortionCoefficient()[i];
            }
            intrinsicParam[9] = cam1->getSkew();
            arlCamera1.setIntrinsic(intrinsicParam);

            intrinsicParam[0] = cam2->getFx();
            intrinsicParam[1] = cam2->getFy();
            intrinsicParam[2] = cam2->getCx();
            intrinsicParam[3] = cam2->getCy();
            for (int i = 0; i < 5; ++i)
            {
                intrinsicParam[i + 4] = cam2->getDistortionCoefficient()[i];
            }
            intrinsicParam[9] = cam2->getSkew();
        }

        arlCamera2.setIntrinsic(intrinsicParam);

        ::arlCore::extrinsicCalibration(models, points, cameras, ext_matrices, opti, log);

#if (SPYLOG_LEVEL >= 5)

        for(::arlCore::vnl_rigid_matrix m : ext_matrices)
        {
            SLM_DEBUG("Result matrix: " + m.getString());
        }

        int i = 0;
        for(double val : log)
        {
            OSLM_DEBUG("log " << i << ": " << val);
            ++i;
        }
#endif
        ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();

        ext_matrices[ext_matrices.size() - 1].copy_out(&(matrix->getCoefficients()[0]));
        {
            ::fwData::mt::ObjectWriteLock camSeriesLock(camSeries);
            camSeries->setExtrinsicMatrix(m_camIndex, matrix);
        }

        ::arData::CameraSeries::ExtrinsicCalibratedSignalType::sptr sig;
        sig = camSeries->signal< ::arData::CameraSeries::ExtrinsicCalibratedSignalType > (
            ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
