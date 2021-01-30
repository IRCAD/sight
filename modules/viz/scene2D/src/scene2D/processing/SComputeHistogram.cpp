/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "scene2D/processing/SComputeHistogram.hpp"

#include "scene2D/processing/ComputeHistogramFunctor.hxx"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Histogram.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <services/macros.hpp>

#include <boost/lexical_cast.hpp>

fwServicesRegisterMacro( ::sight::services::IController, ::scene2D::processing::SComputeHistogram)

namespace scene2D
{
namespace processing
{

static const services::IService::KeyType s_IMAGE_INPUT     = "image";
static const services::IService::KeyType s_HISTOGRAM_INPUT = "histogram";

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
    const std::vector < core::runtime::ConfigurationElement::sptr > binsWidthCfg = m_configuration->find("binsWidth");
    SLM_ASSERT("Missing tag 'binsWidth'", !binsWidthCfg.empty());

    const std::string binsWidth = binsWidthCfg.front()->getValue();
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
    data::Image::csptr image = this->getInput< data::Image>(s_IMAGE_INPUT);

    data::mt::ObjectReadLock imgLock(image);

    if(data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        data::Histogram::sptr histogram = this->getInOut< data::Histogram>(s_HISTOGRAM_INPUT);

        data::mt::ObjectWriteLock lock(histogram);

        ComputeHistogramFunctor::Parameter param;
        param.image     = image;
        param.histogram = histogram;
        param.binsWidth = m_binsWidth;

        core::tools::Type type = image->getType();
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, ComputeHistogramFunctor >::invoke( type,
                                                                                                           param );

        auto sig = histogram->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
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

services::IService::KeyConnectionsMap SComputeHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace processing
} // namespace scene2D
