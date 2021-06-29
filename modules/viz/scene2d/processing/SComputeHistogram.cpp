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

#include "modules/viz/scene2d/processing/SComputeHistogram.hpp"

#include "modules/viz/scene2d/processing/ComputeHistogramFunctor.hxx"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Histogram.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <service/macros.hpp>

#include <boost/lexical_cast.hpp>

namespace sight::module::viz::scene2d
{

namespace processing
{

static const service::IService::KeyType s_IMAGE_INPUT     = "image";
static const service::IService::KeyType s_HISTOGRAM_INPUT = "histogram";

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
    const ConfigType config = this->getConfigTree();
    m_binsWidth = config.get<float>("binsWidth");
}

//-----------------------------------------------------------------------------

void SComputeHistogram::starting()
{
    this->update();
}

//-----------------------------------------------------------------------------

void SComputeHistogram::updating()
{
    const auto image = this->getLockedInput<data::Image>(s_IMAGE_INPUT);

    if(data::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
    {
        auto histogram = this->getLockedInOut<data::Histogram>(s_HISTOGRAM_INPUT);

        ComputeHistogramFunctor::Parameter param;
        param.image     = image.get_shared();
        param.histogram = histogram.get_shared();
        param.binsWidth = m_binsWidth;

        core::tools::Type type = image->getType();
        core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, ComputeHistogramFunctor>::invoke(
            type,
            param
        );

        auto sig = histogram->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void SComputeHistogram::stopping()
{
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SComputeHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace processing

} // namespace sight::module::viz::scene2d
