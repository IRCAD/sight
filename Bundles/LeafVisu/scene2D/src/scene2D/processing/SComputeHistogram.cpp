/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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

fwServicesRegisterMacro( ::fwServices::IController, ::scene2D::processing::SComputeHistogram);

namespace scene2D
{
namespace processing
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT     = "image";
static const ::fwServices::IService::KeyType s_HISTOGRAM_INPUT = "histogram";

SComputeHistogram::SComputeHistogram() noexcept :
    m_binsWidth(1.0f)
{
}

//-----------------------------------------------------------------------------

SComputeHistogram::~SComputeHistogram() noexcept
{
}

//-----------------------------------------------------------------------------

void SComputeHistogram::configuring()
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > binsWidthCfg = m_configuration->find("binsWidth");
    SLM_ASSERT("Missing tag 'binsWidth'", !binsWidthCfg.empty());

    std::string binsWidth = binsWidthCfg.front()->getValue();
    SLM_ASSERT("'binsWidth' must not be empty", !binsWidth.empty());
    m_binsWidth = ::boost::lexical_cast<float>(binsWidth);
}

//-----------------------------------------------------------------------------

void SComputeHistogram::starting()
{
    m_slotUpdate->asyncRun();
}

//-----------------------------------------------------------------------------

void SComputeHistogram::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image>(s_IMAGE_INOUT);

    ::fwData::mt::ObjectReadLock imgLock(image);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwData::Histogram::sptr histogram = this->getInOut< ::fwData::Histogram>(s_HISTOGRAM_INPUT);

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

void SComputeHistogram::swapping()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SComputeHistogram::stopping()
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SComputeHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace processing
} // namespace scene2D

