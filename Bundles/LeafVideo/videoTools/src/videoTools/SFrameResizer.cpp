/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SFrameResizer.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace videoTools
{

fwServicesRegisterMacro( ::fwServices::IOperator, ::videoTools::SFrameResizer, ::fwData::Composite);

//------------------------------------------------------------------------------
SFrameResizer::SFrameResizer() throw()
{
}

//------------------------------------------------------------------------------

SFrameResizer::~SFrameResizer() throw()
{
}

//------------------------------------------------------------------------------

void SFrameResizer::configuring() throw(::fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr config;

    config = m_configuration->findConfigurationElement("input");
    SLM_ASSERT("Missing input timeline in SFrameResizer configuration", config);
    m_inputTLKey = config->getValue();

    config = m_configuration->findConfigurationElement("output");
    SLM_ASSERT("Missing output timeline in SFrameResizer configuration", config);
    m_outputTLKey = config->getValue();

    config = m_configuration->findConfigurationElement("ratio");
    SLM_ASSERT("Missing ratio in SFrameResizer configuration", config);
    const std::string x = config->getAttributeValue("x");
    const std::string y = config->getAttributeValue("y");

    m_ratio[0] = strtof(x.c_str(), NULL) / 100.f;
    m_ratio[1] = strtof(y.c_str(), NULL) / 100.f;

    config = m_configuration->findConfigurationElement("interpolation");

    if(config)
    {
        m_interpolationMethod = ::cv::INTER_NEAREST;

        const std::string method = config->getValue();
        if(method == "linear")
        {
            m_interpolationMethod = ::cv::INTER_LINEAR;
        }
        else if(method == "area")
        {
            m_interpolationMethod = ::cv::INTER_AREA;
        }
        else if(method == "cubic")
        {
            m_interpolationMethod = ::cv::INTER_CUBIC;
        }
        else if(method == "lanczos4")
        {
            m_interpolationMethod = ::cv::INTER_LANCZOS4;
        }
        else if(method != "nearest")
        {
            OSLM_ERROR("Unknown '" << method << "' interpolation method. Defaulting to nearest filter." );
        }
    }
}

//------------------------------------------------------------------------------

void SFrameResizer::starting() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    m_inputTL = ::arData::FrameTL::dynamicCast((*composite)[m_inputTLKey]);
    OSLM_ASSERT("The timeline \"" << m_inputTL << "\" is not valid.", m_inputTL);

    m_outputTL = ::arData::FrameTL::dynamicCast((*composite)[m_outputTLKey]);
    OSLM_ASSERT("The timeline \"" << m_outputTL << "\" is not valid.", m_outputTL);

    m_connections.connect(m_inputTL, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, this->getSptr(), s_UPDATE_SLOT);
}

//------------------------------------------------------------------------------

void SFrameResizer::stopping() throw(::fwTools::Failed)
{
    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void SFrameResizer::updating() throw(::fwTools::Failed)
{
    // Retrieves the last timestamp
    ::fwCore::HiResClock::HiResClockType timestamp = m_inputTL->getNewerTimestamp();

    // Get the buffer of the copied timeline
    CSPTR(::arData::FrameTL::BufferType) bufferFrameIn = m_inputTL->getClosestBuffer(timestamp);
    const ::boost::uint8_t* frameBuffIn = &bufferFrameIn->getElement(0);

    int width  = static_cast<int>( m_inputTL->getWidth() );
    int height = static_cast<int>( m_inputTL->getHeight() );

    // The cv image that will be processed
    ::cv::Mat imgIn(::cv::Size(width, height), CV_8UC4, (void*)frameBuffIn, ::cv::Mat::AUTO_STEP);

    size_t outWidth  = static_cast<size_t>( m_inputTL->getWidth() * m_ratio[0] );
    size_t outHeight = static_cast<size_t>( m_inputTL->getHeight() * m_ratio[1] );

    if((outWidth != m_outputTL->getWidth() || outHeight != m_outputTL->getHeight()))
    {
        m_outputTL->clearTimeline();
        m_outputTL->initPoolSize(outWidth, outHeight, ::fwTools::Type::s_UINT8, 4);
    }

    // Get the buffer of the timeline to fill
    SPTR(::arData::FrameTL::BufferType) bufferOut = m_outputTL->createBuffer(timestamp);
    ::boost::uint8_t* frameBuffOut                = bufferOut->addElement(0);

    // Create an openCV mat that aliases the buffer created from the output timeline
    ::cv::Size size(static_cast<int>(outWidth), static_cast<int>(outHeight));
    ::cv::Mat imgOut(size, CV_8UC4, (void*)frameBuffOut, ::cv::Mat::AUTO_STEP);

    ::cv::resize(imgIn, imgOut, size, 0, 0, m_interpolationMethod );

    m_outputTL->pushObject(bufferOut);

    auto sig =
        m_outputTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG);
    sig->asyncEmit(timestamp);
}

//------------------------------------------------------------------------------

} // namespace videoTools
