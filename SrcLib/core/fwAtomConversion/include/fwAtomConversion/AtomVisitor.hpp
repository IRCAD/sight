/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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

/**
 * @brief This class is used to convert a fwAtoms to a fwData.
 * @class AtomVisitor
 * @date 2013
 */
class FWATOMCONVERSION_CLASS_API AtomVisitor
{

public:

    typedef std::map< ::fwTools::UUID::UUIDType, ::fwData::Object::sptr > DataCacheType;

    /// Constructors. Initializes parameters.
    FWATOMCONVERSION_API AtomVisitor( ::fwAtoms::Object::sptr atomObj, DataCacheType & cache );

    /// Destructor. Does nothing.
    FWATOMCONVERSION_API virtual ~AtomVisitor();

    /**
     * @brief Visits the atom information to create the data object and store it in the cache.
     *
     * Creates a new ::fwData::Object from classname store in meta info CLASSNAME_METAINFO.
     * Creates this new object with UUID store in fwAtoms::Object ID.
     *
     * @throw ::fwAtomConversion::exception::DataFactoryNotFound if the data class is not found in ::fwData::factory
     * @throw ::fwAtomConversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
     */
    FWATOMCONVERSION_API void visit();

    /// Returns the data object. Calls this method after visit().
    FWATOMCONVERSION_API ::fwData::Object::sptr getDataObject() const;

private:

    /// Creates an empty data object from the classname in atom info and store it in the cache.
    void processMetaInfos( const ::fwAtoms::Object::MetaInfosType & metaInfos );

    /// Visits data object to fill it from atom (use AtomToDataMappingVisitor).
    void processAttributes( const ::fwAtoms::Object::AttributesType & attributes );

    /// Atom object to convert
    ::fwAtoms::Object::sptr m_atomObj;

    /// Converted data object
    ::fwData::Object::sptr m_dataObj;

    /// Cache to register the atoms already converted, used when an atom is referenced multiple times.
    DataCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_ATOMVISITOR_HPP__
