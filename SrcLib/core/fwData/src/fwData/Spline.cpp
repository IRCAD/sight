/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Spline.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Spline, ::fwData::Spline );
namespace fwData
{
//------------------------------------------------------------------------------

Spline::Spline()
{
	SLM_WARN("::fwData::Spline()");
}

//------------------------------------------------------------------------------

Spline::~Spline()
{
	SLM_WARN("::fwData::~Spline()");
	m_points.clear();
}

//------------------------------------------------------------------------------

Spline::Points &Spline::points()
{
	return m_points ;
}

//------------------------------------------------------------------------------

} // namespace fwData
