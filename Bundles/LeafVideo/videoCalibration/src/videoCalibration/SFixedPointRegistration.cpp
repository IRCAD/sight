/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SFixedPointRegistration.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Array.hpp>

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Array.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include <arlcore/PointsList.h>


namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SFixedPointRegistration,
                        ::fwData::TransformationMatrix3D);


// ----------------------------------------------------------------------------

SFixedPointRegistration::SFixedPointRegistration() throw () :
    m_pointListUID("")
{

}

// ----------------------------------------------------------------------------

SFixedPointRegistration::~SFixedPointRegistration() throw ()
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::configuring() throw (fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType PLConfig = m_configuration->findConfigurationElement("pointList");
    SLM_ASSERT("element 'pointList' is missing.", PLConfig);
    m_pointListUID = PLConfig->getValue();
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::starting() throw (fwTools::Failed)
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_pointListUID);
    SLM_ASSERT("Object '" + m_pointListUID + "' is not found", obj);
    m_pointList = ::fwData::PointList::dynamicCast(obj);
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::swapping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::updating() throw (fwTools::Failed)
{
    m_calibrationMatrix = this->getObject< ::fwData::TransformationMatrix3D >();

    ::arlCore::PointList::sptr arlPL = ::arlCore::PointList::New();

    ::fwData::PointList::PointListContainer plcontainer = m_pointList->getPoints();
    //for all points in pointlist
    for(unsigned int i = 0; i< plcontainer.size(); ++i)
    {
        double x = plcontainer[i]->getCoord()[0]; //x
        double y = plcontainer[i]->getCoord()[1]; //y
        double z = plcontainer[i]->getCoord()[2]; //z
        //put in arlcore
        arlPL->push_back(x,y,z);
    }

    //Since all points of the pointlist sould be on a aproximate half sphere we can estimate the center of the sphere
    ::arlCore::Point::sptr centerEstimation = ::arlCore::Point::New();
    double radius;
    std::vector<double> params;
    std::vector<double> log;
    arlPL->sphereCenterEstimation(centerEstimation,radius,
                                  arlCore::ARLCORE_SCE_OPTIMIZATION,params, log);


    //TODO: estimation of the rotation between marker and tool tip

    //put the distance into matrix
    m_calibrationMatrix->setCoefficient(0,3,centerEstimation->x());
    m_calibrationMatrix->setCoefficient(1,3,centerEstimation->y());
    m_calibrationMatrix->setCoefficient(2,3,centerEstimation->z());


    OSLM_DEBUG("Sphere center : "<<centerEstimation->x()<<" ; "<<centerEstimation->y()<<" ; "<<centerEstimation->z());

    auto sig = m_calibrationMatrix->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);
    sig->asyncEmit();
}

// ----------------------------------------------------------------------------

void SFixedPointRegistration::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

} //namespace videoCalibration

