/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/Spline.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::Spline );

namespace fwData
{

//------------------------------------------------------------------------------

Spline::Spline(::fwData::Object::Key key)
{
    FW_DEPRECATED_MSG("This data is deprecated and will be removed, use ::fwData::PointList instead");
}

//------------------------------------------------------------------------------

Spline::~Spline()
{
    SLM_TRACE_FUNC();
    m_points.clear();
}

//------------------------------------------------------------------------------

Spline::Points& Spline::points()
{
    return m_points;
}

//------------------------------------------------------------------------------

void Spline::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    Spline::csptr other = Spline::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    SLM_FATAL("Not implemented." );
}

//------------------------------------------------------------------------------

} // namespace fwData
