/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "scene2D/processing/SComputeHistogram.hpp"

#include "scene2D/processing/ComputeHistogramFunctor.hxx"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

fwServicesRegisterMacro( ::fwServices::IController, ::scene2D::processing::SComputeHistogram, ::fwData::Image );

namespace scene2D
{
namespace processing
{

SComputeHistogram::SComputeHistogram() throw() : m_binsWidth(1.0f)
{
}

//-----------------------------------------------------------------------------

SComputeHistogram::~SComputeHistogram() throw()
{
}

//-----------------------------------------------------------------------------

void SComputeHistogram::configuring() throw ( ::fwTools::Failed )
{
    if(!this->isVersion2())
    {
        std::vector < ::fwRuntime::ConfigurationElement::sptr > cfg = m_configuration->find("histogramId");
        SLM_ASSERT("Missing tag 'histogramId'", !cfg.empty());

        m_histogramId = cfg.front()->getValue();
        SLM_ASSERT("'histogramId' must not be empty", !m_histogramId.empty());
    }

    std::vector < ::fwRuntime::ConfigurationElement::sptr > binsWidthCfg = m_configuration->find("binsWidth");
    SLM_ASSERT("Missing tag 'binsWidth'", !binsWidthCfg.empty());

    std::string binsWidth = binsWidthCfg.front()->getValue();
    SLM_ASSERT("'binsWidth' must not be empty", !binsWidth.empty());
    m_binsWidth = ::boost::lexical_cast<float>(binsWidth);
}

//-----------------------------------------------------------------------------

void SComputeHistogram::starting() throw ( ::fwTools::Failed )
{
    m_slotUpdate->asyncRun();
}

//-----------------------------------------------------------------------------

void SComputeHistogram::updating() throw ( ::fwTools::Failed )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::mt::ObjectReadLock imgLock(image);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwData::Histogram::sptr histogram = this->getHistogram();

        ::fwData::mt::ObjectWriteLock lock(histogram);

        ComputeHistogramFunctor::Parameter param;
        param.image     = image;
        param.histogram = histogram;
        param.binsWidth = m_binsWidth;

        ::fwTools::DynamicType type = image->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ComputeHistogramFunctor >::invoke( type, param );

        auto sig = histogram->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void SComputeHistogram::swapping() throw ( ::fwTools::Failed )
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SComputeHistogram::stopping() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

::fwData::Histogram::sptr SComputeHistogram::getHistogram()
{
    ::fwData::Histogram::sptr histogram;
    if(!this->isVersion2())
    {
        SLM_ASSERT("Object " << m_histogramId << " doesn't exist", ::fwTools::fwID::exist(m_histogramId));

        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_histogramId);
        histogram                   = ::fwData::Histogram::dynamicCast(obj);
        SLM_ASSERT("Object " << m_histogramId << " is not a '::fwData::Histogram'", histogram);
    }
    else
    {
        histogram = this->getInOut< ::fwData::Histogram>("histogram");
    }

    return histogram;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SComputeHistogram::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SComputeHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( "image", ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( "image", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace processing
} // namespace scene2D



