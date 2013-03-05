/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__
#define  __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

struct PathVisitor
{
    typedef ::boost::shared_ptr<PathVisitor> sptr;
    typedef std::vector<std::string> ObjectsNamesType;

    ObjectsNamesType m_vectObj;
    ObjectsNamesType m_vectObjFound;

    PathVisitor(const std::string& seshatPath)
    {
        ::boost::split( m_vectObj, seshatPath, ::boost::is_any_of("."));
    }

    void merge(PathVisitor::sptr pathVisitor)
    {
        const ObjectsNamesType& vectObjFound =  pathVisitor->m_vectObjFound;
        m_vectObjFound.reserve(m_vectObjFound.size() + vectObjFound.size());
        m_vectObjFound.insert(m_vectObjFound.end(), vectObjFound.begin(), vectObjFound.end());
    }

    void addObject(const std::string& objPath)
    {
        m_vectObjFound.push_back(objPath);
    }

    bool allObjectsFound() const
    {
        return m_vectObj == m_vectObjFound;
    }
};

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

    FWATOMCONVERSION_API bool objectsFound() const;

    PathVisitor::sptr getPathVisitor() const
    {
        return m_pathVisitor;
    }

private :

    /// Returns after visit process, the subObject given by m_subObjPath
    ::fwData::Object::sptr getSubObject();

    /*
     * @brief Parses m_newSubObjPath, returns the substring until the first dot ( property name, key map or
     * index array ) and updates m_newSubObjPath.
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

    /// Object retrieve after introspection
    ::fwData::Object::sptr m_subObject;

protected:

    PathVisitor::sptr m_pathVisitor;

};

/// Returns the subObject of an object given by subObjPath thanks to camp reflection
FWATOMCONVERSION_API ::fwData::Object::sptr getSubObject( ::fwData::Object::sptr object,
                                                          const std::string & subObjPath,
                                                          bool raiseException = false);

/// Returns the subObject of an object given by subObjPath thanks to camp reflection
template< class DATATYPE >
SPTR(DATATYPE) getSubObject( ::fwData::Object::sptr object,
                             const std::string & subObjPath,
                             bool raiseException = false )
{
    ::fwData::Object::sptr subObject = getSubObject( object, subObjPath, raiseException );
    SPTR(DATATYPE) casteDdata = ::boost::dynamic_pointer_cast<DATATYPE>( subObject );
    return casteDdata;
}

}

#endif // __FWATOMCONVERSION_RETREIVEOBJECTVISITOR_HPP__
