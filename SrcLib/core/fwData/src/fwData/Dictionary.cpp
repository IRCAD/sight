/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"



#include "fwData/Dictionary.hpp"

fwDataRegisterMacro( ::fwData::Dictionary );
namespace fwData
{
//------------------------------------------------------------------------------

Dictionary::Dictionary()
{
}

//------------------------------------------------------------------------------

Dictionary::~Dictionary()
{
}


//------------------------------------------------------------------------------

void Dictionary::shallowCopy( Dictionary::csptr _source )
{
    ::fwData::Object::fieldShallowCopy( _source );

    m_attrDictionaryOrgans = _source->m_attrDictionaryOrgans;
}

//------------------------------------------------------------------------------

void Dictionary::deepCopy( Dictionary::csptr _source )
{
    ::fwData::Object::fieldDeepCopy( _source );

    m_attrDictionaryOrgans.clear();
    BOOST_FOREACH(DictionaryOrganContainerType::value_type element, _source->m_attrDictionaryOrgans)
    {
        m_attrDictionaryOrgans[element.first] = ::fwData::Object::copy(element.second);
    }
}



//------------------------------------------------------------------------------

bool Dictionary::hasDictionaryOrgan( const DictionaryOrganNameType & dictionaryOrganName ) const
{
    const DictionaryOrganContainerType &organs = this->getDictionaryOrgans();
    return organs.find(dictionaryOrganName) != organs.end() ;
}

//------------------------------------------------------------------------------

void Dictionary::setDictionaryOrgan( ::fwData::DictionaryOrgan::sptr _dictionaryOrgan )
{
    SLM_ASSERT("Null _dictionaryOrgan pointer", _dictionaryOrgan);
    DictionaryOrganContainerType::value_type newValue(_dictionaryOrgan->getStructureType(), _dictionaryOrgan);
    std::pair<DictionaryOrganContainerType::iterator, bool> res;
    res = m_attrDictionaryOrgans.insert(newValue);
    if( !res.second )
    {
        res.first->second = _dictionaryOrgan;
    }
}

//------------------------------------------------------------------------------

::fwData::DictionaryOrgan::sptr Dictionary::getDictionaryOrgan( const std::string & dictionaryOrganName )
{
    ::fwData::DictionaryOrgan::sptr organ;
    DictionaryOrganContainerType::iterator iter;
    iter = m_attrDictionaryOrgans.find(dictionaryOrganName);

    if (iter != m_attrDictionaryOrgans.end())
    {
        organ = iter->second;
    }

    return organ;
}

//------------------------------------------------------------------------------

} // end namespace fwData
