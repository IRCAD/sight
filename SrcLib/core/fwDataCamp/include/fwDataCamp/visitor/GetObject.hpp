/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATACAMP_VISITOR_GETOBJECT_HPP__
#define  __FWDATACAMP_VISITOR_GETOBJECT_HPP__

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwDataCamp/config.hpp"

namespace fwDataCamp
{

namespace visitor
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
 * @class   GetObject
 * @brief   This class is an helper to introspect a data and find an object (contained into this data) from a normalized path.
 *
 * @date    2013.
 */
class FWDATACAMP_CLASS_API GetObject : public ::camp::ExtendedClassVisitor
{

public:

    FWDATACAMP_API GetObject( ::fwData::Object::sptr object, const std::string & subObjPath );

    FWDATACAMP_API virtual ~GetObject();

    FWDATACAMP_API void visit(const camp::SimpleProperty& property);
    FWDATACAMP_API void visit(const camp::EnumProperty& property);
    FWDATACAMP_API void visit(const camp::UserProperty& property);
    FWDATACAMP_API void visit(const camp::ArrayProperty& property);
    FWDATACAMP_API void visit(const camp::Function& function);
    FWDATACAMP_API virtual void visit(const camp::MapProperty& property);

    /// Launches visit process and returns the object designated by m_subObjPath
    FWDATACAMP_API ::fwData::Object::sptr get();

    FWDATACAMP_API bool objectsFound() const;

    PathVisitor::sptr getPathVisitor() const
    {
        return m_pathVisitor;
    }

private :

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

} // namespace visitor

} // namespace fwDataCamp

#endif // __FWDATACAMP_VISITOR_GETOBJECT_HPP__

