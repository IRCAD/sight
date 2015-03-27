/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SMatrixTLSelector.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <extData/MatrixTL.hpp>
#include <extData/FrameTL.hpp>
#include <extData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/helper/Array.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <functional>

fwServicesRegisterMacro(::videoTools::ISynchronizer, ::videoTools::SMatrixTLSelector, ::fwData::Composite);

namespace videoTools
{

// ----------------------------------------------------------------------------

SMatrixTLSelector::SMatrixTLSelector() throw () :
    m_lastTimestamp(0),
    m_tolerance(500),
    m_imagesInitialized(false),
    m_timeStep(33)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSelector::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    ConfigurationType framesConfig = m_configuration->findConfigurationElement("frames");
    SLM_FATAL_IF("Missing \"frames\" tag.", !framesConfig);

    std::vector< ConfigurationType > frameCfgs = framesConfig->find("frame");
    for(ConfigurationType cfg : frameCfgs)
    {
        std::string frameTL = cfg->getAttributeValue("from");
        SLM_ASSERT("Missing attribute 'from'", !frameTL.empty() );
        std::string frame = cfg->getAttributeValue("to");
        SLM_ASSERT("Missing attribute 'to'", !frame.empty());
        m_frameKeys[frameTL] = frame;
    }

    ConfigurationType matricesConfig = m_configuration->findConfigurationElement("matrices");
    SLM_FATAL_IF("Missing \"matrices\" tag.", !matricesConfig);

    if (matricesConfig)
    {
        std::vector< ConfigurationType > matrixCfgs = matricesConfig->find("matrix");
        SLM_ASSERT("Missing 'matrix' tag", !matrixCfgs.empty());
        for(ConfigurationType elt : matrixCfgs)
        {
            MatrixCfgType matrixCfg;
            const std::string index = elt->getAttributeValue("index");
            matrixCfg.m_index = ::boost::lexical_cast< size_t >(index);

            const std::string matrix = elt->getAttributeValue("to");
            SLM_ASSERT("Missing attribute 'index' and/or 'to'", !index.empty() && !matrix.empty() );

            matrixCfg.m_matrixID = matrix;

            std::vector< ConfigurationType > timelineCfgs = elt->find("timeline");
            SLM_ASSERT("Missing 'timeline' tag", !timelineCfgs.empty());

            for(ConfigurationType timelineCfg : timelineCfgs)
            {
                const std::string matrixTL = timelineCfg->getAttributeValue("from");
                SLM_ASSERT("Missing attribute 'from'", !matrixTL.empty());
                matrixCfg.m_timelineKeyVect.push_back(matrixTL);
            }

            m_matrixCfgVector.push_back(matrixCfg);
        }
    }

    ConfigurationType framerateConfig = m_configuration->findConfigurationElement("framerate");
    SLM_WARN_IF("Missing \"framerate\" tag.", !framerateConfig );
    if(framerateConfig)
    {
        m_timeStep = 1000 / ::boost::lexical_cast< unsigned int >(framerateConfig->getValue());
    }

}

// ----------------------------------------------------------------------------

void SMatrixTLSelector::starting() throw (fwTools::Failed)
{
    ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();
    for(FrameKeysType::value_type elt : m_frameKeys)
    {
        ::extData::FrameTL::sptr frameTL = comp->at< ::extData::FrameTL >(elt.first);
        ::fwData::Image::sptr frame      = comp->at< ::fwData::Image >(elt.second);

        m_frameTLs[elt.first] = frameTL;
        m_images[elt.first]   = frame;
    }

    for(auto& matrixConfig : m_matrixCfgVector)
    {
        matrixConfig.m_matrix = comp->at< ::fwData::TransformationMatrix3D >(matrixConfig.m_matrixID);
        for(auto timelineKey : matrixConfig.m_timelineKeyVect)
        {
            matrixConfig.m_timelineObjectMap[timelineKey] = comp->at< ::extData::MatrixTL >(timelineKey);
        }
    }


    SLM_ASSERT("No valid worker for timer.", m_associatedWorker);
    m_timer                                      = m_associatedWorker->createTimer();
    ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(m_timeStep);
    m_timer->setFunction( std::bind( &SMatrixTLSelector::synchronize, this)  );
    m_timer->setDuration(duration);
    m_timer->start();

}

// ----------------------------------------------------------------------------

void SMatrixTLSelector::stopping() throw (fwTools::Failed)
{
    m_timer->stop();

    m_frameTLs.clear();
    m_images.clear();
    m_frameKeys.clear();
    m_frameTLs.clear();
    m_matrixCfgVector.clear();
}

// ----------------------------------------------------------------------------

void SMatrixTLSelector::synchronize()
{
    ::fwCore::HiResClock::HiResClockType frameTimestamp =
        std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

    // Get timestamp for synchronization
    for(FrameTLKeyType::value_type elt : m_frameTLs)
    {
        ::fwCore::HiResClock::HiResClockType tlTimestamp = elt.second->getNewerTimestamp();

        if (tlTimestamp == 0)
        {
            SLM_INFO("no available frame for timeline '" + elt.first + "'.");
            return;
        }
        frameTimestamp = std::min(frameTimestamp, tlTimestamp);
    }

    this->updateFrames(frameTimestamp);

    for(auto matrixCfg : m_matrixCfgVector)
    {
        ::fwCore::HiResClock::HiResClockType currentTimestamp =
            std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

        ::extData::MatrixTL::sptr matrixTL;

        for(auto elt : matrixCfg.m_timelineObjectMap)
        {
            ::fwCore::HiResClock::HiResClockType tlTimestamp = elt.second->getNewerTimestamp();
            int delta = std::numeric_limits< int >::max();
            if(tlTimestamp > 0)
            {
                int tmpDelta = std::abs(frameTimestamp - tlTimestamp);
                if( tmpDelta < m_tolerance)
                {
                    if(tmpDelta < delta)
                    {
                        delta            = tmpDelta;
                        currentTimestamp = tlTimestamp;
                        matrixTL         = elt.second;
                    }
                }
            }
            else
            {
                SLM_INFO("no available matrix for timeline '" + elt.first + "'.");
            }
        }

        if(matrixTL)
        {
            CSPTR(::extData::MatrixTL::BufferType) buffer = matrixTL->getBuffer(currentTimestamp);
            if(buffer)
            {
                size_t index = matrixCfg.m_index;
                if(index < buffer->getMaxElementNum())
                {
                    if(buffer->isPresent(index))
                    {
                        const float* values = buffer->getElement(index);

                        ::fwData::TransformationMatrix3D::sptr matrix = matrixCfg.m_matrix;
                        for(unsigned int i = 0; i < 4; ++i)
                        {
                            for(unsigned int j = 0; j < 4; ++j)
                            {
                                matrix->setCoefficient(i,j,values[i*4+j]);
                            }
                        }

                        ::fwComEd::TransformationMatrix3DMsg::sptr msg = ::fwComEd::TransformationMatrix3DMsg::New();
                        msg->addEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);
                        msg->setSubject(matrix);
                        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
                        sig = matrix->signal< ::fwData::Object::ObjectModifiedSignalType >(
                            ::fwData::Object::s_OBJECT_MODIFIED_SIG);
                        sig->asyncEmit(msg);
                    }
                }
                else
                {
                    SLM_WARN("Index of matrix in XML configuration is out of range.");
                }
            }
        }
    }


}

// ----------------------------------------------------------------------------

void SMatrixTLSelector::updateFrames(::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwComEd::ImageMsg::sptr msg = ::fwComEd::ImageMsg::New();
    msg->addEvent(::fwComEd::ImageMsg::BUFFER);
    for(FrameTLKeyType::value_type elt : m_frameTLs)
    {
        ::extData::FrameTL::sptr frameTL = elt.second;
        ::fwData::Image::sptr image      = m_images[elt.first];

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
            ::fwData::Image::OriginType origin(3,0);

            image->setOrigin(origin);
            ::fwData::Image::SpacingType spacing(3,voxelSize);
            image->setSpacing(spacing);
            image->setWindowWidth(1);
            image->setWindowCenter(0);

            m_imagesInitialized = true;

            //Notify (needed for instance to update the texture in ::visuVTKARAdaptor::SVideoAdapter)
            msg->addEvent(::fwComEd::ImageMsg::MODIFIED);
        }

        ::fwData::mt::ObjectWriteLock destLock(image);
        ::fwData::Array::sptr array = image->getDataArray();

        ::fwComEd::helper::Array arrayHelper(array);

        CSPTR(::extData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

        if(!buffer)
        {
            OSLM_INFO("Buffer not found with timestamp "<< timestamp);
            return;
        }


        const ::boost::uint8_t* frameBuff = &buffer->getElement(0);
        ::boost::uint8_t* index = arrayHelper.begin< ::boost::uint8_t >();
        std::copy( frameBuff, frameBuff+buffer->getSize(), index);

        //Notify
        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = image->signal< ::fwData::Object::ObjectModifiedSignalType >(
            ::fwData::Object::s_OBJECT_MODIFIED_SIG );
        sig->asyncEmit(msg);
    }
}

// ----------------------------------------------------------------------------


}  // namespace videoTools
