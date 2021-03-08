/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/Matrix4.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA( sight::data::Matrix4 );

namespace sight::data
{

//------------------------------------------------------------------------------

Matrix4::Matrix4(data::Object::Key)
{
    //default initialization
    m_vCoefficients.fill(0.);
    for(size_t t = 0; t < MATRIX_SIZE; ++t)
    {
        m_vCoefficients[t*MATRIX_SIZE + t] = 1.;
    }
}

//------------------------------------------------------------------------------

Matrix4::~Matrix4()
{
}

//-----------------------------------------------------------------------------

void Matrix4::shallowCopy(const Object::csptr& _source )
{
    Matrix4::csptr other = Matrix4::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vCoefficients = other->m_vCoefficients;
}

//-----------------------------------------------------------------------------

void Matrix4::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Matrix4::csptr other = Matrix4::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_vCoefficients = other->m_vCoefficients;
}

//------------------------------------------------------------------------------

} // namespace sight::data
