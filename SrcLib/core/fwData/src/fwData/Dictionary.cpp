/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>



#include "fwData/Dictionary.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Dictionary, ::fwData::Dictionary );
namespace fwData
{
const Object::FieldID Dictionary::ID_DICTIONARY_ORGANS = "ID_DICTIONARY_ORGANS";

//------------------------------------------------------------------------------

Dictionary::Dictionary()
{
        SLM_WARN("HACK POUR LA SERIALISATION => Dictionary() is not a protected methode (problem with the singleton concept.)");
        SLM_WARN("::fwData::Dictionary() : (ToDo) field default value");
        setField( Dictionary::ID_DICTIONARY_ORGANS );
}

//------------------------------------------------------------------------------

Dictionary::~Dictionary()
{
        SLM_WARN("::fwData::~Dictionary() : (ToDo)");
}

//------------------------------------------------------------------------------

bool Dictionary::hasDictionaryOrgan( const std::string & dictionaryOrganName ) const
{
        const Object::ChildContainer m_vOrgans = this->getField( Dictionary::ID_DICTIONARY_ORGANS )->children();
        for (   Object::ChildContainer::const_iterator organ = m_vOrgans.begin();
                        organ != m_vOrgans.end();
                        ++organ )
        {
                if( boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan>(*organ)->getStructureType() == dictionaryOrganName )
                {
                        return true;
                }
        }

        return false;
}

//------------------------------------------------------------------------------

void Dictionary::setDictionaryOrgan( ::fwData::DictionaryOrgan::sptr _dictionaryOrgan )
{
        bool definitionFound = false;

        Object::ChildContainer m_vOrgans = this->getField( Dictionary::ID_DICTIONARY_ORGANS )->children();

        for (   Object::ChildContainer::iterator organ = m_vOrgans.begin();
                        organ != m_vOrgans.end() && !definitionFound;
                        ++organ )
        {
                if( boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan>(*organ)->getStructureType() == _dictionaryOrgan->getStructureType() )
                {
                        *organ = _dictionaryOrgan;
                        definitionFound = true;
                        SLM_DEBUG("Dictionary::setDictionaryOrgan : Update a dictionary organ");
                }
        }

        // If it is a new defintion
        if( !definitionFound )
        {
                this->addFieldElement( Dictionary::ID_DICTIONARY_ORGANS , _dictionaryOrgan );
                SLM_DEBUG("Dictionary::setDictionaryOrgan : Add a dictionary organ");
        }

}

//------------------------------------------------------------------------------

::fwData::DictionaryOrgan::sptr Dictionary::getDictionaryOrgan( const std::string & dictionaryOrganName )
{
        Object::ChildContainer m_vOrgans = this->getField( Dictionary::ID_DICTIONARY_ORGANS )->children();
        for (   Object::ChildContainer::iterator organ = m_vOrgans.begin();
                        organ != m_vOrgans.end();
                        ++organ )
        {
                if( boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan>(*organ)->getStructureType() == dictionaryOrganName )
                {
                        return boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan>(*organ);
                }
        }

        // If not found build a null shared_ptr
        ::fwData::DictionaryOrgan::sptr defaultOrgan;
        return defaultOrgan;
}

//------------------------------------------------------------------------------

std::pair< Dictionary::DictionaryOrganIterator, Dictionary::DictionaryOrganIterator >
Dictionary::getDictionaryOrgans()
{
        DictionaryOrganIterator begin(  getField( Dictionary::ID_DICTIONARY_ORGANS )->children().begin() );
        DictionaryOrganIterator   end(  getField( Dictionary::ID_DICTIONARY_ORGANS )->children().end()   );
        return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< Dictionary::DictionaryOrganConstIterator, Dictionary::DictionaryOrganConstIterator >
Dictionary::getDictionaryOrgans() const
{
        DictionaryOrganConstIterator begin(  getField( Dictionary::ID_DICTIONARY_ORGANS )->children().begin()   );
        DictionaryOrganConstIterator   end(  getField( Dictionary::ID_DICTIONARY_ORGANS )->children().end()   );
        return std::make_pair( begin, end );
}

} // end namespace fwData
