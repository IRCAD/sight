/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Series.hpp"
#include "fwMedData/SeriesContainer.hpp"


namespace fwMedData
{

SeriesContainer::SeriesContainer(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

SeriesContainer::~SeriesContainer()
{}

//------------------------------------------------------------------------------

void SeriesContainer::shallowCopy(SeriesContainer::csptr _src)
{
    this->fieldShallowCopy( _src );

    m_attrContainer= _src->m_attrContainer;
}

//------------------------------------------------------------------------------

void SeriesContainer::deepCopy(SeriesContainer::csptr _src)
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

