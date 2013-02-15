/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__
#define  __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

/**
 * @class   RetreiveObjectVisitor
 * @brief   This class is an helper to introspect a data and find a subObject from a normalized path
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2012-2012.
 */
class FWATOMCONVERSION_CLASS_API RetreiveObjectVisitor : public ::camp::ExtendedClassVisitor
{

public:

    FWATOMCONVERSION_API RetreiveObjectVisitor( ::fwData::Object::sptr object, const std::string & subObjPath );

    FWATOMCONVERSION_API virtual ~RetreiveObjectVisitor();

    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);
    FWATOMCONVERSION_API void visit(const camp::Function& function);
    FWATOMCONVERSION_API virtual void visit(const camp::MapProperty& property);

    /// Launches visit process and return the subObject given by m_subObjPath
    FWATOMCONVERSION_API ::fwData::Object::sptr retreive();

private :

    /// Returns after visit process, the subObject given by m_subObjPath
    ::fwData::Object::sptr getSubObject();

    /*
     * @brief Parses m_newSubObjPath, returns the substring until the first dot ( property name, key map or
     * index array ) and udaptes m_newSubObjPath.
     */
    std::string getNextPropertyName();

    /// Object given in constructor which will introspected
    ::fwData::Object::sptr m_object;

    /// Path of final object wanted from m_object
    const std::string m_subObjPath;

    /// Path of final object wanted from m_object without first property name
    std::string m_newSubObjPath;

    /// First m_object property wanted
    std::string m_propertyName;

    /// Reflection in camp world of m_object
    ::camp::UserObject m_campObj;

    /// Object retreive after introspection
    ::fwData::Object::sptr m_subObject;

};

/// Returns the subObject of an object given by subObjPath thanks to camp reflection
FWATOMCONVERSION_API ::fwData::Object::sptr getSubObject( ::fwData::Object::sptr object,
                                                          const std::string & subObjPath );

/// Returns the subObject of an object given by subObjPath thanks to camp reflection
template< class DATATYPE >
SPTR(DATATYPE) getSubObject( ::fwData::Object::sptr object, const std::string & subObjPath )
{
    ::fwData::Object::sptr subObject = getSubObject( object, subObjPath );
    SPTR(DATATYPE) casteDdata = ::boost::dynamic_pointer_cast<DATATYPE>( subObject );
    return casteDdata;
}

}

#endif // __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__
