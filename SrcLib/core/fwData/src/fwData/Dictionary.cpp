/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"



#include "fwData/Dictionary.hpp"

fwDataRegisterMacro( ::fwData::Dictionary );
namespace fwData
{
//------------------------------------------------------------------------------

Dictionary::Dictionary( ::fwData::Object::Key key )
{
}

//------------------------------------------------------------------------------

Dictionary::~Dictionary()
{
}


//------------------------------------------------------------------------------

void Dictionary::shallowCopy(const Object::csptr &_source )
{
    Dictionary::csptr other = Dictionary::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    ::fwData::Object::fieldShallowCopy( other );

    m_attrDictionaryOrgans = other->m_attrDictionaryOrgans;
}

//------------------------------------------------------------------------------

void Dictionary::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Dictionary::csptr other = Dictionary::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( other, cache );

    m_attrDictionaryOrgans.clear();
    BOOST_FOREACH(DictionaryOrganContainerType::value_type element, other->m_attrDictionaryOrgans)
    {
        m_attrDictionaryOrgans[element.first] = ::fwData::Object::copy(element.second, cache);
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
