/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/AtomVisitor.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomToDataMappingVisitor.hpp"
#include "fwAtomConversion/exception/DataFactoryNotFound.hpp"
#include "fwAtomConversion/exception/DuplicatedDataUUID.hpp"

namespace fwAtomConversion
{

AtomVisitor::AtomVisitor( ::fwAtoms::Object::sptr atomObj, DataCacheType & cache )
: m_atomObj ( atomObj ),
  m_cache ( cache )
{}

AtomVisitor::~AtomVisitor()
{}

void AtomVisitor::visit()
{
    this->processMetaInfos( m_atomObj->getMetaInfos() );
    this->processAttributes( m_atomObj->getAttributes() );
}

void AtomVisitor::processMetaInfos( const ::fwAtoms::Object::MetaInfosType & metaInfos )
{
    const DataVisitor::ClassnameType & classname = metaInfos.find( DataVisitor::CLASSNAME_METAINFO )->second;
    ::fwTools::UUID::UUIDType uuid = m_atomObj->getId();
    m_dataObj = ::fwData::factory::New(classname);

    std::stringstream msg;
    msg << "Data object '"<< classname << "' not found in data factory during atom to data conversion";
    FW_RAISE_EXCEPTION_IF( exception::DataFactoryNotFound(msg.str()), ! m_dataObj );

    bool uuidIsSetted = ::fwTools::UUID::set(m_dataObj, uuid);
    msg << "Try to create new data object '"
        << classname << "' with uuid '"
        << uuid <<"' but this uuid is already used in the system";
    FW_RAISE_EXCEPTION_IF( exception::DuplicatedDataUUID(msg.str()), ! uuidIsSetted );

    m_cache[uuid] = m_dataObj;
}

void AtomVisitor::processAttributes( const ::fwAtoms::Object::AttributesType & attributes )
{
    const camp::Class& metaclass = ::camp::classByName( m_dataObj->getClassname() );
    ::fwAtomConversion::AtomToDataMappingVisitor visitor ( m_dataObj, m_atomObj, m_cache );
    metaclass.visit(visitor);
}

::fwData::Object::sptr AtomVisitor::getDataObject() const
{
    return m_dataObj;
}

} // end namespace fwAtomConversion
