/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Spline.hpp"

REGISTER_DATA( ::fwData::Spline );
namespace fwData
{
//------------------------------------------------------------------------------

Spline::Spline()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Spline::~Spline()
{
    SLM_TRACE_FUNC();
    m_points.clear();
}

//------------------------------------------------------------------------------

Spline::Points &Spline::points()
{
    return m_points ;
}

//------------------------------------------------------------------------------

} // namespace fwData
