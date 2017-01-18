/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SFrameMatrixSynchronizer.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <algorithm>
#include <functional>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::videoTools::SFrameMatrixSynchronizer, ::fwData::Composite);

namespace videoTools
{

const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_DONE_SIG = "synchronizationDone";

const ::fwServices::IService::KeyType s_FRAMETL_INPUT  = "frameTL";
const ::fwServices::IService::KeyType s_MATRIXTL_INPUT = "matrixTL";
const ::fwServices::IService::KeyType s_IMAGE_INOUT    = "image";
const ::fwServices::IService::KeyType s_MATRICES_INOUT = "matrices";

// ----------------------------------------------------------------------------

SFrameMatrixSynchronizer::SFrameMatrixSynchronizer() throw () :
    m_tolerance(500),
    m_imagesInitialized(false),
    m_timeStep(33)
{
    m_sigSynchronizationDone = newSignal<SynchronizationDoneSignalType>(s_SYNCHRONIZATION_DONE_SIG);
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    if(!this->isVersion2())
    {
        ConfigurationType framesConfig = m_configuration->findConfigurationElement("frames");
        SLM_FATAL_IF("Missing \"frames\" tag.", !framesConfig);

        std::vector< ConfigurationType > frameCfgs = framesConfig->find("frame");
        for(ConfigurationType cfg : frameCfgs)
        {
            std::string frameTL = cfg->getAttributeValue("from");
            std::string frame   = cfg->getAttributeValue("to");
            SLM_ASSERT("Missing attribute 'from' and/or 'to'", !frameTL.empty() && !frame.empty());
            m_frameKeys[frameTL] = frame;
        }

        ConfigurationType matricesConfig = m_configuration->findConfigurationElement("matrices");
        SLM_WARN_IF("Missing \"matrices\" tag.", !matricesConfig);

        if (matricesConfig)
        {
            std::vector< ConfigurationType > timelineCfgs = matricesConfig->find("timeline");
            SLM_ASSERT("Missing 'timeline' tag", !timelineCfgs.empty());

            for(ConfigurationType timelineCfg : timelineCfgs)
            {
                const std::string matrixTL = timelineCfg->getAttributeValue("from");
                SLM_ASSERT("Missing attribute 'from'", !matrixTL.empty());

                MatrixVectorType& matrixVector              = m_matrixKeys[matrixTL];
                std::vector< ConfigurationType > matrixCfgs = timelineCfg->find("matrix");

                SLM_ASSERT("Missing 'matrix' tag", !matrixCfgs.empty() );

                for(ConfigurationType cfg : matrixCfgs)
                {
                    const std::string index = cfg->getAttributeValue("index");
                    const int id            = ::boost::lexical_cast< int >(index);

                    const std::string matrix = cfg->getAttributeValue("to");

                    SLM_ASSERT("Missing attribute 'index' and/or 'to'", !index.empty() && !matrix.empty() );

                    matrixVector.push_back(std::make_pair(matrix, id));
                }
            }
        }
    }

    ConfigurationType framerateConfig = m_configuration->findConfigurationElement("framerate");
    SLM_WARN_IF("Missing \"framerate\" tag.", !framerateConfig );
    if(framerateConfig)
    {
        m_timeStep = 1000 / ::boost::lexical_cast< unsigned int >(framerateConfig->getValue());
    }

    ConfigurationType toleranceConfig = m_configuration->findConfigurationElement("tolerance");
    if(toleranceConfig)
    {
        m_tolerance = ::boost::lexical_cast< unsigned int >(toleranceConfig->getValue());
    }

}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::starting() throw (fwTools::Failed)
{
    if(this->isVersion2())
    {
        const size_t numFrameTLs = this->getKeyGroupSize(s_FRAMETL_INPUT);
        const size_t numImages   = this->getKeyGroupSize(s_IMAGE_INOUT);
        SLM_ASSERT("You should have the same number of 'frameTL' and 'image' keys", numFrameTLs == numImages);

        for(size_t i = 0; i < numFrameTLs; ++i)
        {
            // TODO: replace by a vector when appXml is removed
            m_frameTLs["frame" + std::to_string(i)] = this->getInput< ::arData::FrameTL>(s_FRAMETL_INPUT, i);
            m_images["frame" + std::to_string(i)]   = this->getInOut< ::fwData::Image>(s_IMAGE_INOUT, i);
        }

        const size_t numMatrixTLs = this->getKeyGroupSize(s_MATRIXTL_INPUT);

        for(size_t i = 0; i < numMatrixTLs; ++i)
        {
            // Get the group corresponding to the 'i' Matrix TimeLine. The name of this group is matrices0 for example.
            // if ever the group is not found 'getKeyGroupSize' will assert.
            const size_t numMatrices = this->getKeyGroupSize(s_MATRICES_INOUT+std::to_string(i));

            m_matrixTLs["matrixTL" + std::to_string(i)] = this->getInput< ::arData::MatrixTL>(s_MATRIXTL_INPUT, i);

            MatrixKeyVectorType matricesVector;
            for(size_t j = 0; j < numMatrices; ++j)
            {
                matricesVector.push_back(
                    std::make_pair(this->getInOut< ::fwData::TransformationMatrix3D >(
                                       s_MATRICES_INOUT+std::to_string(i), j), j));
            }
            m_matrices["matrixTL" + std::to_string(i)] = matricesVector;
        }
    }
    else
    {
        ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();
        for(FrameKeysType::value_type elt : m_frameKeys)
        {
            ::arData::FrameTL::sptr frameTL = comp->at< ::arData::FrameTL >(elt.first);
            SLM_ASSERT("::arData::FrameTL is not valid", frameTL);
            ::fwData::Image::sptr frame = comp->at< ::fwData::Image >(elt.second);
            SLM_ASSERT("::fwData::Image is not valid", frame);

            m_frameTLs[elt.first] = frameTL;
            m_images[elt.first]   = frame;
        }

        for(MatrixKeysType::value_type elt : m_matrixKeys)
        {
            ::arData::MatrixTL::sptr matrixTL = comp->at< ::arData::MatrixTL >(elt.first);

            m_matrixTLs[elt.first] = matrixTL;
            MatrixKeyVectorType& matrixVector = m_matrices[elt.first];

            for(MatrixVectorType::value_type mat : elt.second)
            {
                ::fwData::TransformationMatrix3D::sptr matrix =
                    comp->at< ::fwData::TransformationMatrix3D >(mat.first);
                matrixVector.push_back(std::make_pair(matrix, mat.second));
            }
        }
    }

    SLM_ASSERT("No valid worker for timer.", m_associatedWorker);
    m_timer                                      = m_associatedWorker->createTimer();
    ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(m_timeStep);
    m_timer->setFunction( std::bind( &SFrameMatrixSynchronizer::synchronize, this)  );
    m_timer->setDuration(duration);
    m_timer->start();

}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::stopping() throw (fwTools::Failed)
{
    m_timer->stop();

    m_frameTLs.clear();
    m_images.clear();
    m_matrixTLs.clear();
    m_matrices.clear();
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::synchronize()
{
    // Timestamp reference for the synchronization
    ::fwCore::HiResClock::HiResClockType frameTimestamp = 0;

    typedef std::pair< ::fwCore::HiResClock::HiResClockType, std::string> TimeLinePairType;
    typedef std::vector< TimeLinePairType > TimelineType;
    typedef std::vector< std::string > MatrixTimelineType;

    // Get timestamp for synchronization
    TimelineType availableFramesTL;
    availableFramesTL.reserve(m_frameTLs.size());

    // If multiple TLs are set, we want to synchronize their frames together.
    // If TLs are updated, we get the one with the oldest timestamp to synchronize them.
    // In particular case, we could have only one TL updated, we still need to get frames from it.
    // Then we get the one with the newest timestamp and the other ones are not considered.
    for(FrameTLKeyType::value_type elt : m_frameTLs)
    {
        ::fwCore::HiResClock::HiResClockType tlTimestamp = elt.second->getNewerTimestamp();
        if(tlTimestamp > 0)
        {
            // Check if the current TL timestamp and the previous one are closed enough (according to the tolerance)
            if (std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
            {
                // Sets the reference timestamp as the minimum value
                frameTimestamp = std::min(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(std::make_pair(tlTimestamp, elt.first));
            }
            // Otherwise keep the most recent timestamp as frameTimestamp
            else
            {
                // If the difference between the TLs timestamp is superior to the tolerance
                // we set the reference timestamp as the maximum of them
                frameTimestamp = std::max(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(std::make_pair(tlTimestamp, elt.first));

                // Now remove all frames that are too far away
                availableFramesTL.erase(std::remove_if(availableFramesTL.begin(), availableFramesTL.end(),
                                                       [ = ](const TimeLinePairType& tl)
                    {
                        return std::abs(frameTimestamp - tl.first) >= m_tolerance;
                    }), availableFramesTL.end());
            }
        }
        else
        {
            SLM_INFO("no available matrix for timeline '" + elt.first + "'.");
        }
    }

    ::fwCore::HiResClock::HiResClockType matrixTimestamp = frameTimestamp;

    MatrixTimelineType availableMatricesTL;
    availableMatricesTL.reserve(m_matrixTLs.size());
    for(MatrixTLKeyType::value_type elt : m_matrixTLs)
    {
        ::fwCore::HiResClock::HiResClockType tlTimestamp = elt.second->getNewerTimestamp();
        if(tlTimestamp > 0)
        {
            if (std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
            {
                matrixTimestamp = std::min(matrixTimestamp, tlTimestamp);
                availableMatricesTL.push_back(elt.first);
            }
        }
        else
        {
            SLM_INFO("no available matrix for timeline '" + elt.first + "'.");
        }
    }

    for(TimelineType::value_type key : availableFramesTL)
    {
        ::arData::FrameTL::csptr frameTL = m_frameTLs[key.second];
        ::fwData::Image::sptr image      = m_images[key.second];

        ::fwData::Image::SizeType size(3);
        size[0] = frameTL->getWidth();
        size[1] = frameTL->getHeight();
        size[2] = 1;
        // Check if image dimensions has changed
        if(size != image->getSize())
        {
            m_imagesInitialized = false;
        }

        if(!m_imagesInitialized)
        {
            const ::fwData::Image::SpacingType::value_type voxelSize = 1;
            image->allocate(size, frameTL->getType(), frameTL->getNumberOfComponents());
            ::fwData::Image::OriginType origin(3, 0);

            image->setOrigin(origin);
            ::fwData::Image::SpacingType spacing(3, voxelSize);
            image->setSpacing(spacing);
            image->setWindowWidth(1);
            image->setWindowCenter(0);

            m_imagesInitialized = true;

            // Notify (needed for instance to update the texture in ::visuVTKARAdaptor::SVideoAdapter)
            auto sig = image->signal< ::fwData::Image::ModifiedSignalType >(
                ::fwData::Image::s_MODIFIED_SIG );
            sig->asyncEmit();
        }

        ::fwData::mt::ObjectWriteLock destLock(image);
        ::fwData::Array::sptr array = image->getDataArray();

        ::fwDataTools::helper::Array arrayHelper(array);

        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(matrixTimestamp);

        if(!buffer)
        {
            OSLM_INFO("Buffer not found with timestamp "<< matrixTimestamp);
            return;
        }

        const ::boost::uint8_t* frameBuff = &buffer->getElement(0);
        ::boost::uint8_t* index = arrayHelper.begin< ::boost::uint8_t >();
        std::copy( frameBuff, frameBuff+buffer->getSize(), index);

        // Notify
        auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(
            ::fwData::Image::s_BUFFER_MODIFIED_SIG );
        sig->asyncEmit();
    }

    bool matrixFound = false;
    for(MatrixTimelineType::value_type key : availableMatricesTL)
    {
        ::arData::MatrixTL::csptr matrixTL           = m_matrixTLs[key];
        CSPTR(::arData::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(matrixTimestamp);

        if(buffer)
        {
            const MatrixKeyVectorType& matrixVector = m_matrices[key];

            for(unsigned int k = 0; k < matrixVector.size(); ++k)
            {
                const MatrixKeyVectorTypePair& matrixKey = matrixVector[k];

                if(matrixKey.second < buffer->getMaxElementNum())
                {
                    if(buffer->isPresent(matrixKey.second))
                    {
                        const float* values = buffer->getElement(matrixKey.second);

                        ::fwData::TransformationMatrix3D::sptr matrix = matrixKey.first;

                        for(unsigned int i = 0; i < 4; ++i)
                        {
                            for(unsigned int j = 0; j < 4; ++j)
                            {
                                matrix->setCoefficient(i, j, values[i*4+j]);
                            }
                        }

                        auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                            ::fwData::Object::s_MODIFIED_SIG);
                        sig->asyncEmit();

                        matrixFound = true;
                    }
                }
                else
                {
                    SLM_WARN("Index of matrix in XML configuration is out of range.");
                }
            }
        }
    }

    if (matrixFound)
    {
        m_sigSynchronizationDone->asyncEmit(frameTimestamp);
    }
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::updating() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

}  // namespace videoTools
