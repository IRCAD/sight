/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/Image.hpp>
#include "fwData/registry/macros.hpp"

#include "fwMedData/ModelSeries.hpp"

fwDataRegisterMacro( ::fwMedData::ModelSeries );

namespace fwMedData
{

ModelSeries::ModelSeries(::fwData::Object::Key key): Series(key)
{
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries()
{}

//------------------------------------------------------------------------------

void ModelSeries::shallowCopy(ModelSeries::csptr _src)
{
    this->fieldShallowCopy( _src );

    m_attrReconstructionDB = _src->m_attrReconstructionDB;
}

//------------------------------------------------------------------------------

void ModelSeries::deepCopy(ModelSeries::csptr _src)
{
    this->fieldDeepCopy( _src );

    m_attrReconstructionDB = _src->m_attrReconstructionDB;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

