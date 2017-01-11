/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

SProbePosition::SProbePosition() throw()
{
    m_sigPositionChanged = newSignal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

SProbePosition::~SProbePosition() throw()
{
}

//------------------------------------------------------------------------------

void SProbePosition::starting() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbePosition::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbePosition::configuring() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbePosition::updating() throw(::fwTools::Failed)
{
    const int s_minI = 95;
    const int s_maxI = 194;
    const int s_minJ = 160;
    const int s_maxJ = 259;

    ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D>();
    int y = static_cast<int>( matrix->getCoefficient(1,3) );
    int z = static_cast<int>( matrix->getCoefficient(2,3) );

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
