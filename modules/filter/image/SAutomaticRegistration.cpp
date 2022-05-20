/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "SAutomaticRegistration.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <filter/image/AutomaticRegistration.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/ProgressDialog.hpp>

#include <chrono>
#include <fstream>
#include <iomanip>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

SAutomaticRegistration::SAutomaticRegistration() :
    IHasParameters(m_slots)
{
}

//------------------------------------------------------------------------------

SAutomaticRegistration::~SAutomaticRegistration()
{
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    m_minStep = config.get<double>("minStep", -1.);

    SIGHT_FATAL_IF("Invalid or missing minStep.", m_minStep <= 0);

    m_maxIterations = config.get<unsigned long>("maxIterations", 0);

    SIGHT_FATAL_IF("Invalid or missing number of iterations.", m_maxIterations == 0);

    const std::string metric = config.get<std::string>("metric", "");
    this->setMetric(metric);

    const std::string shrinkList = config.get<std::string>("levels", "");
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

        SIGHT_ASSERT("There must be two parameters: shrink and sigma.", parameters.size() == 2);

        const unsigned long shrink = std::stoul(parameters[0]);
        const double sigma         = std::stod(parameters[1]);

        m_multiResolutionParameters.push_back(std::make_pair(shrink, sigma));
    }

    if(m_multiResolutionParameters.empty())
    {
        // By default, no multi-resolution
        m_multiResolutionParameters.push_back(std::make_pair(1, 0.0));
    }

    m_samplingPercentage = config.get<double>("samplingPercentage", 1.);

    m_log = config.get<bool>("log", false);
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::starting()
{
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::updating()
{
    using sight::filter::image::AutomaticRegistration;

    const auto target    = m_target.lock();
    const auto reference = m_reference.lock();

    auto transform = m_transform.lock();

    SIGHT_ASSERT("No " << s_TARGET_IN << " found !", target);
    SIGHT_ASSERT("No " << s_REFERENCE_IN << " found !", reference);
    SIGHT_ASSERT("No " << s_TRANSFORM_INOUT << " found !", transform);

    // Create a copy of m_multiResolutionParameters without empty values
    AutomaticRegistration::MultiResolutionParametersType
        multiResolutionParameters(m_multiResolutionParameters.size());

    typedef AutomaticRegistration::MultiResolutionParametersType::value_type ParamPairType;

    auto lastElt = std::remove_copy_if(
        m_multiResolutionParameters.begin(),
        m_multiResolutionParameters.end(),
        multiResolutionParameters.begin(),
        [](const ParamPairType& v){return v.first == 0;});

    multiResolutionParameters.erase(lastElt, multiResolutionParameters.end());

    AutomaticRegistration registrator;

    sight::ui::base::dialog::ProgressDialog dialog("Automatic Registration", "Registering, please be patient.");

    dialog.setCancelCallback(
        [&registrator]()
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

    auto transfoModifiedSig = transform->signal<data::Matrix4::ModifiedSignalType>
                                  (data::Matrix4::s_MODIFIED_SIG);

    std::chrono::time_point<std::chrono::high_resolution_clock> regStartTime;
    std::size_t i = 0;

    AutomaticRegistration::IterationCallbackType iterationCallback =
        [&]()
        {
            const itk::SizeValueType currentIteration = registrator.getCurrentIteration();
            const itk::SizeValueType currentLevel     = registrator.getCurrentLevel();

            const float progress = float(i++) / float(m_maxIterations * multiResolutionParameters.size());

            std::string msg = "Number of iterations : " + std::to_string(i) + " Current level : "
                              + std::to_string(currentLevel);
            dialog(progress, msg);
            dialog.setMessage(msg);

            registrator.getCurrentMatrix(transform.get_shared());

            if(m_log)
            {
                std::stringstream transformStream;

                for(std::uint8_t j = 0 ; j < 16 ; ++j)
                {
                    transformStream << transform->getCoefficients()[j];

                    if(j != 15)
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
        registrator.registerImage(
            target.get_shared(),
            reference.get_shared(),
            transform.get_shared(),
            m_metric,
            multiResolutionParameters,
            m_samplingPercentage,
            m_minStep,
            m_maxIterations,
            iterationCallback
        );
    }
    catch(itk::ExceptionObject& e)
    {
        SIGHT_ERROR("[ITK EXCEPTION]" << e.GetDescription());
    }

    m_sigComputed->asyncEmit();
    transfoModifiedSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::stopping()
{
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SAutomaticRegistration::getAutoConnections() const
{
    return {
        {s_TARGET_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_TARGET_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_REFERENCE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_REFERENCE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_TRANSFORM_INOUT, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT}
    };
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
        SIGHT_FATAL("Key must be 'metric', unknown key :" << key);
    }
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setDoubleParameter(double val, std::string key)
{
    if(key == "minStep")
    {
        m_minStep = val;
    }
    else if(key.find("sigma_") != std::string::npos)
    {
        const unsigned long level = this->extractLevelFromParameterName(key);
        m_multiResolutionParameters[level].second = val;
    }
    else if(key == "samplingPercentage")
    {
        m_samplingPercentage = val;
    }
    else
    {
        SIGHT_FATAL("Unknown key : " << key);
    }
}

//------------------------------------------------------------------------------

void SAutomaticRegistration::setIntParameter(int val, std::string key)
{
    if(key == "maxIterations")
    {
        SIGHT_FATAL_IF("The number of iterations must be greater than 0 !!", val <= 0);
        m_maxIterations = static_cast<unsigned long>(val);
    }
    else if(key.find("shrink_") != std::string::npos)
    {
        const unsigned long level = this->extractLevelFromParameterName(key);
        m_multiResolutionParameters[level].first = itk::SizeValueType(val);
    }
    else
    {
        SIGHT_FATAL("Unknown key : " << key);
    }
}

//------------------------------------------------------------------------------
unsigned long SAutomaticRegistration::extractLevelFromParameterName(const std::string& name)
{
    // find the level
    const std::string levelSuffix = name.substr(name.find("_") + 1);
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
        m_metric = sight::filter::image::MEAN_SQUARES;
    }
    else if(metricName == "NormalizedCorrelation")
    {
        m_metric = sight::filter::image::NORMALIZED_CORRELATION;
    }
    else if(metricName == "MutualInformation")
    {
        m_metric = sight::filter::image::MUTUAL_INFORMATION;
    }
    else
    {
        SIGHT_FATAL("Unknown metric: " << metricName);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
