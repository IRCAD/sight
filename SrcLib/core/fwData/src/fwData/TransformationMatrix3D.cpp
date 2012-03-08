/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/TransformationMatrix3D.hpp"

fwDataRegisterMacro( ::fwData::TransformationMatrix3D );

namespace fwData
{

//------------------------------------------------------------------------------

TransformationMatrix3D::TransformationMatrix3D() :
m_vCoefficients(16)
{
    //default init
    for(int l = 0; l < 4; l++)
        for(int c = 0; c < 4; c++){
            if(l==c)
                setCoefficient(l, c, 1);
            else
                setCoefficient(l, c, 0);
        }
}

//------------------------------------------------------------------------------

TransformationMatrix3D::~TransformationMatrix3D()
{}

//-----------------------------------------------------------------------------

void TransformationMatrix3D::shallowCopy( TransformationMatrix3D::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    m_vCoefficients = _source->m_vCoefficients;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3D::deepCopy( TransformationMatrix3D::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    m_vCoefficients = _source->m_vCoefficients;
}


//------------------------------------------------------------------------------

double TransformationMatrix3D::getCoefficient(int l, int c)
{
    unsigned int pos = l * MATRIX_SIZE + c;
    assert(pos < m_vCoefficients.size());
    return m_vCoefficients.at(pos);
}

//------------------------------------------------------------------------------

void TransformationMatrix3D::setCoefficient(int l, int c, TransformationMatrix3D::TM3DType val)
{
    unsigned int pos = l * MATRIX_SIZE + c;
    assert(pos < m_vCoefficients.size());
    m_vCoefficients.at(pos)= val ;
}

//------------------------------------------------------------------------------

} // namespace fwData
