/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Image.hpp>
#include <fwData/Histogram.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/HistogramMsg.hpp>

#include "scene2D/processing/SComputeHistogram.hpp"
#include "scene2D/processing/ComputeHistogramFunctor.hxx"

fwServicesRegisterMacro( ::fwServices::IController, ::scene2D::processing::SComputeHistogram, ::fwData::Image );

namespace scene2D
{
namespace processing
{

SComputeHistogram::SComputeHistogram() throw() :
    m_binsWidth(1.0)
{
}

//-----------------------------------------------------------------------------

SComputeHistogram::~SComputeHistogram() throw()
{
}

//-----------------------------------------------------------------------------

void SComputeHistogram::configuring() throw ( ::fwTools::Failed )
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > cfg = m_configuration->find("histogramId");
    SLM_ASSERT("Missing tag 'histogramId'", !cfg.empty());

    m_histogramId = cfg.front()->getValue();
    SLM_ASSERT("'histogramId' must not be empty", !m_histogramId.empty());

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

    if(::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwData::Histogram::sptr histogram = this->getHistogram();

        ::fwData::mt::ObjectWriteLock lock(histogram);

        ComputeHistogramFunctor::Parameter param;
        param.image     = image;
        param.histogram = histogram;
        param.binsWidth = m_binsWidth;

        ::fwTools::DynamicType type = image->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ComputeHistogramFunctor >::invoke( type, param );

        ::fwComEd::HistogramMsg::sptr msg = ::fwComEd::HistogramMsg::New();
        msg->addEvent(::fwComEd::HistogramMsg::VALUE_IS_MODIFIED);
        ::fwServices::IEditionService::notify(this->getSptr(), histogram, msg);
    }
}

//-----------------------------------------------------------------------------

void SComputeHistogram::receiving( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    if(_msg->hasEvent(::fwComEd::ImageMsg::NEW_IMAGE) ||
       _msg->hasEvent(::fwComEd::ImageMsg::BUFFER) ||
       _msg->hasEvent(::fwComEd::ImageMsg::MODIFIED))
    {
        this->updating();
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
    SLM_ASSERT("Object " << m_histogramId << " doesn't exist", ::fwTools::fwID::exist(m_histogramId));

    ::fwTools::Object::sptr obj         = ::fwTools::fwID::getObject(m_histogramId);
    ::fwData::Histogram::sptr histogram = ::fwData::Histogram::dynamicCast(obj);
    SLM_ASSERT("Object " << m_histogramId << " is not a '::fwData::Histogram'", histogram);

    return histogram;
}

//-----------------------------------------------------------------------------

} // namespace processing
} // namespace scene2D



