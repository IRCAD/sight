/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_DATAVISITOR_HPP__
#define  __FWATOMCONVERSION_DATAVISITOR_HPP__

#include <map>

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include <fwTools/UUID.hpp>

#include <fwData/Object.hpp>

#include <fwAtoms/Base.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

/**
 * @brief Visitor used to convert a fwData to a fwAtoms.
 * @class DataVisitor
 * @date 2013
 */
class FWATOMCONVERSION_CLASS_API DataVisitor : public ::camp::ExtendedClassVisitor
{

public:

    typedef std::map< ::fwTools::UUID::UUIDType, ::fwAtoms::Object::sptr > AtomCacheType;

    typedef std::string ClassnameType;

    /// Key of the meta info to store data object classname
    FWATOMCONVERSION_API static const std::string CLASSNAME_METAINFO;

    /**
     * @brief Converts a ::fwData::Object to a ::fwAtoms::Object.
     * @param dataObj data to convert
     * @param cache   cache to register already converted data, used when a data is referenced multiple times.
     * @return Returns the converted atom.
     * @throw ::camp::ClassNotFound if data class is not found.
     */
    FWATOMCONVERSION_API static ::fwAtoms::Object::sptr convert( ::fwData::Object::sptr dataObj, AtomCacheType & cache );

    /// Constructor. Initializes atom object and store it in the cache.
    FWATOMCONVERSION_API DataVisitor( ::fwData::Object::sptr dataObj, AtomCacheType & cache );

    /// Destructor. Does nothing.
    FWATOMCONVERSION_API virtual ~DataVisitor();

    /**
     * @brief Visit simple property. Visits property value with DataConversionValueVisitor.
     * @todo Hack : problem with size_t conversion.
     */
    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);

    /// Visit enum property. Visits property value with DataConversionValueVisitor.
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);

    /// Visit user property. Visits property value with DataConversionValueVisitor.
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);

    /// Visit array property. Visits all elements in array with DataConversionValueVisitor.
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);

    /// Does nothing
    FWATOMCONVERSION_API void visit(const camp::Function& function);

    /// Visit map property. Visits all elements in map with DataConversionValueVisitor.
    FWATOMCONVERSION_API void visit(const camp::MapProperty& property);

    /// Returns the atom object. Calls this methods after the visit.
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr getAtomObject() const;

private:

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// converted atom object.
    ::fwAtoms::Object::sptr m_atomObj;

    /// cache to register already converted object. Used when a data is referenced several times.
    AtomCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_DATAVISITOR_HPP__
