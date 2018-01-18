/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SAutomaticRegistration.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwServices/macros.hpp>

#include <itkRegistrationOp/AutomaticRegistration.hpp>

#include <chrono>
#include <fstream>
#include <iomanip>

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

    const std::string shrinkList = config.get< std::string >("levels", "");
    std::string sigmaShrinkPair;

    std::istringstream shrinksStream(shrinkList);
    while(std::getline(shrinksStream, sigmaShrinkPair, ';'))
    {
        std::istringstream sigmaShrinkStream(sigmaShrinkPair);
        std::vector<std::string> parameters;
        std::string token;

        while(std::getline(sigmaShrinkStream, token, ':'))
        {
            parameters.push_back(token);
        }

        SLM_ASSERT("There must be two parameters: shrink and sigma.", parameters.size() == 2);

        const unsigned long shrink = std::stoul(parameters[0]);
        const double sigma         = std::stod(parameters[1]);

        m_multiResolutionParameters.push_back(std::make_pair(shrink, sigma));
    }

    if(m_multiResolutionParameters.empty())
    {
        // By default, no multi-resolution
        m_multiResolutionParameters.push_back( std::make_pair( 1, 0.0 ));
    }

    m_samplingPercentage = config.get< double >("samplingPercentage", 1.);

    m_log = config.get< bool >("log", false);
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

    auto lastElt = std::remove_copy_if(m_multiResolutionParameters.begin(),
                                       m_multiResolutionParameters.end(),
                                       multiResolutionParameters.begin(),
                                       [](const ParamPairType& v){return v.first == 0; });

    multiResolutionParameters.erase(lastElt, multiResolutionParameters.end());

    ::itkRegistrationOp::AutomaticRegistration registrator;

    ::fwGui::dialog::ProgressDialog dialog("Automatic Registration", "Registering, please be patient.");

    dialog.setCancelCallback([&registrator]()
        {
            registrator.stopRegistration();
        });

    std::fstream regLog;

    if(m_log)
    {
        std::stringstream fileNameStream;
        const std::time_t systemTime = std::time(nullptr);
        fileNameStream << "registration_" << std::put_time(std::localtime(&systemTime), "%Y-%m-%d_%H-%M-%S") << ".csv";

        regLog.open(fileNameStream.str(), std::ios_base::out);
        regLog << "'Timestamp',"
               << "'Current level',"
               << "'Current iteration',"
               << "'Shrink',"
               << "'Sigma',"
               << "'Current metric value',"
               << "'Current parameters',"
               << "'Current transform',"
               << "'Relaxation factor',"
               << "'Learning rate',"
               << "'Gradient magnitude tolerance',"
               << "'Minimum step size',"
               << "'Maximum number of iterations',"
               << "'Sampling rate',"
               << "'Number of levels'"
               << std::endl;
    }

    auto transfoModifiedSig = transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                                  (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    std::chrono::time_point<std::chrono::high_resolution_clock> regStartTime;
    size_t i = 0;

    ::itkRegistrationOp::AutomaticRegistration::IterationCallbackType iterationCallback =
        [&]()
        {
            const ::itk::SizeValueType currentIteration = registrator.getCurrentIteration();
            const ::itk::SizeValueType currentLevel     = registrator.getCurrentLevel();

            const float progress = float(i++)/float(m_maxIterations * multiResolutionParameters.size());

            std::string msg = "Number of iterations : " + std::to_string(i) + " Current level : "
                              + std::to_string(currentLevel);
            dialog(progress, msg);
            dialog.setMessage(msg);

            registrator.getCurrentMatrix(transform);

            if(m_log)
            {
                std::stringstream transformStream;

                for(std::uint8_t i = 0; i < 16; ++i)
                {
                    transformStream << transform->getRefCoefficients()[i];

                    if(i != 15)
                    {
                        transformStream << ";";
                    }
                }

                const std::chrono::time_point<std::chrono::high_resolution_clock> now =
                    std::chrono::high_resolution_clock::now();

                const auto duration = now - regStartTime;

                regLog << "'" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "',"
                       << "'" << currentLevel << "',"
                       << "'" << currentIteration << "',"
                       << "'" << multiResolutionParameters[currentLevel].first << "',"
                       << "'" << multiResolutionParameters[currentLevel].second << "',"
                       << "'" << registrator.getCurrentMetricValue() << "',"
                       << "'" << registrator.getCurrentParameters() << "',"
                       << "'" << transformStream.str() << "',"
                       << "'" << registrator.getRelaxationFactor() << "',"
                       << "'" << registrator.getLearningRate() << "',"
                       << "'" << registrator.getGradientMagnitudeTolerance() << "',"
                       << "'" << m_minStep << "',"
                       << "'" << m_maxIterations << "',"
                       << "'" << m_samplingPercentage << "',"
                       << "'" << multiResolutionParameters.size() << "'"
                       << std::endl;

                regLog.flush(); // Flush, just to be sure.
            }

            transfoModifiedSig->asyncEmit();
        };

    try
    {
        registrator.registerImage(target, reference, transform, m_metric, multiResolutionParameters,
                                  m_samplingPercentage, m_minStep, m_maxIterations, iterationCallback);
    }
    catch(::itk::ExceptionObject& e)
    {
        OSLM_ERROR("[ITK EXCEPTION]" << e.GetDescription());
    }

    m_sigComputed->asyncEmit();
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
