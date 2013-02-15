/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Series.hpp"
#include "fwMedData/SeriesDB.hpp"

fwDataRegisterMacro( ::fwMedData::SeriesDB );

namespace fwMedData
{

SeriesDB::SeriesDB(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{}

//------------------------------------------------------------------------------

void SeriesDB::shallowCopy(SeriesDB::csptr _src)
{
    this->fieldShallowCopy( _src );

    m_attrContainer= _src->m_attrContainer;
}

//------------------------------------------------------------------------------

void SeriesDB::deepCopy(SeriesDB::csptr _src)
{
    this->fieldDeepCopy( _src );
    m_attrContainer.clear();
    m_attrContainer.resize(_src->m_attrContainer.size());
    std::transform(
            _src->begin(), _src->end(),
            this->begin(),
            &::fwData::Object::copy< ValueType::element_type >
    );
}

//------------------------------------------------------------------------------

} // namespace fwMedData

