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
#include <itkRegistrationOp/RegistrationObserver.hxx>

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

void SAutomaticRegistration::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_minStep = config.get< double >("minStep", -1.);

    OSLM_FATAL_IF("Invalid or missing minStep.", m_minStep <= 0);

    m_maxIterations = config.get< unsigned long >("maxIterations", 0);

    OSLM_FATAL_IF("Invalid or missing number of iterations.", m_maxIterations == 0);

    const std::string metric = config.get< std::string >("metric", "");
    this->setMetric(metric);

    // By default, no multi-resolution
    m_multiResolutionParameters.push_back( std::make_pair( 1, 0.0 ));

    m_samplingPercentage = config.get< double >("samplingPercentage", 1.);
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::starting()
{
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::updating()
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

    // Create a copy of m_multiResolutionParameters without empty values
    ::itkRegistrationOp::AutomaticRegistration::MultiResolutionParametersType
        multiResolutionParameters(m_multiResolutionParameters.size());

    typedef ::itkRegistrationOp::AutomaticRegistration::MultiResolutionParametersType::value_type ParamPairType;

    std::remove_copy_if(m_multiResolutionParameters.begin(),
                        m_multiResolutionParameters.end(),
                        multiResolutionParameters.begin(),
                        [](const ParamPairType& v){return v.first == 0; });

    ::itkRegistrationOp::RegistrationObserver::Pointer observer = ::itkRegistrationOp::RegistrationObserver::New();
    observer->setMaxIterations(m_maxIterations);

    ::itkRegistrationOp::AutomaticRegistration::registerImage(target,
                                                              reference,
                                                              transform,
                                                              m_metric,
                                                              multiResolutionParameters,
                                                              m_samplingPercentage,
                                                              m_minStep,
                                                              m_maxIterations,
                                                              ::itk::Command::Pointer(observer.GetPointer()));

    m_sigComputed->asyncEmit();

    auto transfoModifiedSig = transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                                  (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    transfoModifiedSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::stopping()
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

void SAutomaticRegistration::setDoubleParameter(double val, std::string key)
{
    if(key == "minStep")
    {
        m_minStep = val;
    }
    else if(key.find("sigma_") != std::string::npos )
    {
        const unsigned long level = this->extractLevelFromParameterName(key);
        m_multiResolutionParameters[level].second = val;
    }
    else if( key == "samplingPercentage" )
    {
        m_samplingPercentage = val;
    }
    else
    {
        OSLM_FATAL("Unknown key : " << key);
    }
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setIntParameter(int val, std::string key)
{
    if(key == "maxIterations")
    {
        OSLM_FATAL_IF("The number of iterations must be greater than 0 !!", val <= 0);
        m_maxIterations = static_cast<unsigned long>(val);
    }
    else if(key.find("shrink_") != std::string::npos )
    {
        const unsigned long level = this->extractLevelFromParameterName(key);
        m_multiResolutionParameters[level].first = ::itk::SizeValueType(val);
    }
    else
    {
        OSLM_FATAL("Unknown key : " << key);
    }
}

//------------------------------------------------------------------------------
unsigned long SAutomaticRegistration::extractLevelFromParameterName(const std::string& name)
{
    // find the level
    const std::string levelSuffix = name.substr(name.find("_")+1);
    const unsigned long level     = std::stoul(levelSuffix);

    if(level >= m_multiResolutionParameters.size())
    {
        m_multiResolutionParameters.resize(level + 1, std::make_pair(0, 0.0));
    }

    return level;
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setMetric(const std::string& metricName)
{
    if(metricName == "MeanSquares")
    {
        m_metric = ::itkRegistrationOp::MEAN_SQUARES;
    }
    else if(metricName == "NormalizedCorrelation")
    {
        m_metric = ::itkRegistrationOp::NORMALIZED_CORRELATION;
    }
    else if(metricName == "MutualInformation")
    {
        m_metric = ::itkRegistrationOp::MUTUAL_INFORMATION;
    }
    else
    {
        OSLM_FATAL("Unknown metric: " << metricName);
    }
}

//------------------------------------------------------------------------------

} // namespace opItkRegistration
