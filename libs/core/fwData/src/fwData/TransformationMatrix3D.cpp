/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/TransformationMatrix3D.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::TransformationMatrix3D );

namespace fwData
{

//------------------------------------------------------------------------------

TransformationMatrix3D::TransformationMatrix3D(::fwData::Object::Key)
{
    //default initialization
    m_vCoefficients.fill(0.);
    for(size_t t = 0; t < MATRIX_SIZE; ++t)
    {
        m_vCoefficients[t*MATRIX_SIZE + t] = 1.;
    }
}

//------------------------------------------------------------------------------

TransformationMatrix3D::~TransformationMatrix3D()
{
}

//-----------------------------------------------------------------------------

void TransformationMatrix3D::shallowCopy(const Object::csptr& _source )
{
    TransformationMatrix3D::csptr other = TransformationMatrix3D::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vCoefficients = other->m_vCoefficients;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3D::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    TransformationMatrix3D::csptr other = TransformationMatrix3D::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_vCoefficients = other->m_vCoefficients;
}

//------------------------------------------------------------------------------

} // namespace fwData
