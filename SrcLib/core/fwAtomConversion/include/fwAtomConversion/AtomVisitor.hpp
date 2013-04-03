/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_ATOMVISITOR_HPP__
#define  __FWATOMCONVERSION_ATOMVISITOR_HPP__

#include <map>

#include <fwTools/UUID.hpp>

#include <fwData/Object.hpp>

#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

/// ???
class FWATOMCONVERSION_CLASS_API AtomVisitor
{

public:

    typedef std::map< ::fwTools::UUID::UUIDType, ::fwData::Object::sptr > DataCacheType;

    /// ???
    FWATOMCONVERSION_API static ::fwData::Object::sptr convert( ::fwAtoms::Object::sptr atomObj, DataCacheType & cache );

    /// ???
    FWATOMCONVERSION_API AtomVisitor( ::fwAtoms::Object::sptr atomObj, DataCacheType & cache );

    /// ???
    FWATOMCONVERSION_API virtual ~AtomVisitor();

    /// ???
    FWATOMCONVERSION_API void visit();

    /// ???
    FWATOMCONVERSION_API ::fwData::Object::sptr getDataObject() const;

private:

    /// ???
    void processMetaInfos( const ::fwAtoms::Object::MetaInfosType & metaInfos );

    /// ???
    void processAttributes( const ::fwAtoms::Object::AttributesType & attributes );

    /// ???
    ::fwAtoms::Object::sptr m_atomObj;

    /// ???
    ::fwData::Object::sptr m_dataObj;

    /// ???
    DataCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_ATOMVISITOR_HPP__
