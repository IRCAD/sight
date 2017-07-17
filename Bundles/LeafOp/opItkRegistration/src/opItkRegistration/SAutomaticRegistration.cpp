/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SAutomaticRegistration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <itkRegistrationOp/AutomaticRegistration.hpp>

namespace opItkRegistration
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opItkRegistration::SAutomaticRegistration, ::fwData::Image);

static const ::fwServices::IService::KeyType s_TARGET_IN    = "target";
static const ::fwServices::IService::KeyType s_REFERENCE_IN = "reference";

static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

SAutomaticRegistration::SAutomaticRegistration()
{

}

//------------------------------------------------------------------------------

SAutomaticRegistration::~SAutomaticRegistration()
{

}

//------------------------------------------------------------------------------

void SAutomaticRegistration::configuring() throw( ::fwTools::Failed )
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_minStep = config.get< double >("service.minStep", -1.);
    m_maxStep = config.get< double >("service.maxStep", -1.);

    OSLM_FATAL_IF("Invalid or missing minStep.", m_minStep <= 0);
    OSLM_FATAL_IF("Invalid or missing maxStep.", m_maxStep <= 0);

    m_maxIterations = config.get< unsigned long >("service.maxIterations", 0);

    OSLM_FATAL_IF("Invalid or missing number of iterations.", m_maxIterations == 0);

    const std::string metric = config.get< std::string >("service.metric", "");

    setMetric(metric);
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::starting() throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

void SAutomaticRegistration::updating() throw( ::fwTools::Failed )
{
    ::fwData::Image::csptr target    = this->getInput< ::fwData::Image >(s_TARGET_IN);
    ::fwData::Image::csptr reference = this->getInput< ::fwData::Image >(s_REFERENCE_IN);

    ::fwData::mt::ObjectReadLock targetLock(target);
    ::fwData::mt::ObjectReadLock refLock(reference);

    ::fwData::TransformationMatrix3D::sptr transform =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    ::fwData::mt::ObjectWriteLock trfLock(transform);

    SLM_ASSERT("No 'target' found !", target);
    SLM_ASSERT("No 'reference' found !", reference);
    SLM_ASSERT("No 'transform' found !", transform);

    ::itkRegistrationOp::AutomaticRegistration::registerImage(target, reference, transform, m_metric, m_minStep,
                                                              m_maxStep, m_maxIterations);

    m_sigComputed->asyncEmit();

    auto transfoModifiedSig = transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                                  (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    transfoModifiedSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::stopping() throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SAutomaticRegistration::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_TARGET_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TARGET_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_REFERENCE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_REFERENCE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TRANSFORM_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setEnumParameter(std::string val, std::string key)
{
    if(key == "metric")
    {
        setMetric(val);
    }
    else
    {
        OSLM_FATAL("Key must be 'metric', unknown key :" << key);
    }
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setMetric(const std::string& metricName)
{
    if(metricName == "MeanSquares")
    {
        m_metric = ::itkRegistrationOp::AutomaticRegistration::MEAN_SQUARES;
    }
    else if(metricName == "NormalizedCorrelation")
    {
        m_metric = ::itkRegistrationOp::AutomaticRegistration::NORMALIZED_CORRELATION;
    }
    else if(metricName == "MutualInformation")
    {
        m_metric = ::itkRegistrationOp::AutomaticRegistration::MUTUAL_INFORMATION;
    }
    else
    {
        OSLM_FATAL("Unknown metric: " << metricName);
    }
}

//------------------------------------------------------------------------------

} // namespace opItkRegistration
