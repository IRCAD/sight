/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/StructureTraitsDictionary.hpp"

fwDataRegisterMacro( ::fwData::StructureTraitsDictionary );

namespace fwData
{
//------------------------------------------------------------------------------

StructureTraitsDictionary::StructureTraitsDictionary (::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

StructureTraitsDictionary::~StructureTraitsDictionary ()
{
}

//------------------------------------------------------------------------------

StructureTraits::sptr StructureTraitsDictionary::getStructure(std::string type)
{
    OSLM_ASSERT("Structure of type '" << type << "' not found", m_structureTraitsMap.find(
                    type) != m_structureTraitsMap.end());
    return m_structureTraitsMap[type];
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::addStructure(StructureTraits::sptr structureTraits)
{
    std::string type                            = structureTraits->getType();
    StructureTraits::StructureClass structClass = structureTraits->getClass();
    std::string attachment                      = structureTraits->getAttachmentType();
    FW_RAISE_IF("Structure of type '" << type << "' already exist", m_structureTraitsMap.find(
                    type) != m_structureTraitsMap.end());
    FW_RAISE_IF("Structure of class '" << structClass << "' can not have attachment",
                !(attachment.empty() || structClass == StructureTraits::LESION ||
                  structClass == StructureTraits::FUNCTIONAL) );
    FW_RAISE_IF("Structure attachment '" << attachment << "' not found in dictionary",
                !(attachment.empty() || m_structureTraitsMap.find(attachment) != m_structureTraitsMap.end() ) );
    FW_RAISE_IF("Structure attachment '" << attachment << "' must be of class ORGAN",
                !(attachment.empty() || m_structureTraitsMap[attachment]->getClass() == StructureTraits::ORGAN ) );
    FW_RAISE_IF("Structure must have at least one category",
                structureTraits->getCategories().empty());
    FW_RAISE_IF("Wrong structure type '" << type<< "', a type cannot contain space", structureTraits->getType().find(
                    " ") != std::string::npos );

    m_structureTraitsMap[type] = structureTraits;
}

//------------------------------------------------------------------------------

StructureTraitsDictionary::StructureTypeNameContainer StructureTraitsDictionary::getStructureTypeNames() const
{
    StructureTypeNameContainer vectNames;
    std::transform( m_structureTraitsMap.begin(), m_structureTraitsMap.end(),
                    std::back_inserter(vectNames),
                    ::boost::bind(&StructureTraitsMapType::value_type::first,_1) );
    return vectNames;
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::shallowCopy(const Object::csptr &source )
{
    StructureTraitsDictionary::csptr other = StructureTraitsDictionary::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( source );
    m_structureTraitsMap = other->m_structureTraitsMap;
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
{
    StructureTraitsDictionary::csptr other = StructureTraitsDictionary::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );
    m_structureTraitsMap.clear();
    BOOST_FOREACH(StructureTraitsMapType::value_type elt, other->m_structureTraitsMap)
    {
        m_structureTraitsMap[elt.first] = ::fwData::Object::copy(elt.second, cache);
    }
}

//------------------------------------------------------------------------------


} // namespace fwData


