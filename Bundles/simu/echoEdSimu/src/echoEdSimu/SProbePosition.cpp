/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "echoEdSimu/SProbePosition.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SProbePosition, ::fwData::TransformationMatrix3D);

const ::fwCom::Signals::SignalKeyType SProbePosition::s_POSITION_MODIFIED_SIG = "positionModified";

//------------------------------------------------------------------------------

SProbePosition::SProbePosition() noexcept
{
    m_sigPositionChanged = newSignal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

SProbePosition::~SProbePosition() noexcept
{
}

//------------------------------------------------------------------------------

void SProbePosition::starting()
{
    FW_DEPRECATED_MSG("This service is deprecated.", "19.0");
}

//------------------------------------------------------------------------------

void SProbePosition::stopping()
{
}

//------------------------------------------------------------------------------

void SProbePosition::configuring()
{
}

//------------------------------------------------------------------------------

void SProbePosition::updating()
{
    const int s_minI = 95;
    const int s_maxI = 194;
    const int s_minJ = 160;
    const int s_maxJ = 259;

    ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D>();
    int y = static_cast<int>( matrix->getCoefficient(1, 3) );
    int z = static_cast<int>( matrix->getCoefficient(2, 3) );

    int i = z;
    int j = -y;
    // add origin of echo images
    i -= 155;
    j += 210;

    i = std::min( std::max(s_minI, i), s_maxI);
    j = std::min( std::max(s_minJ, j), s_maxJ);

    m_sigPositionChanged->asyncEmit(i, j);
}

//------------------------------------------------------------------------------

} // namespace echoEdSimu
