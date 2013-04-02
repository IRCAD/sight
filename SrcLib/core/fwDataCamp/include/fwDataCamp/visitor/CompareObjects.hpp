/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATACAMP_VISITOR_COMPAREOBJECTS_HPP__
#define  __FWDATACAMP_VISITOR_COMPAREOBJECTS_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwDataCamp/config.hpp"

namespace fwData
{
    class Object;
}

namespace fwDataCamp
{

namespace visitor
{

/**
 * @class   CompareObjects
 * @brief   Visitor used to compare two objects.
 * @date    2013.
 */
class FWDATACAMP_CLASS_API CompareObjects : public ::camp::ExtendedClassVisitor
{

public:

    //typedef std::map< std::string, SPTR(::fwData::Object) > PropsMapType;
    typedef std::map< std::string, std::string > PropsMapType;

    /**
     * @brief Constructor.
     */
    FWDATACAMP_API CompareObjects();

    FWDATACAMP_API virtual ~CompareObjects();

    /**
     * @name Introspection methods implementation
     * @{ */
    FWDATACAMP_API void visit(const camp::SimpleProperty& property);
    FWDATACAMP_API void visit(const camp::EnumProperty& property);
    FWDATACAMP_API void visit(const camp::UserProperty& property);
    FWDATACAMP_API void visit(const camp::ArrayProperty& property);
    FWDATACAMP_API void visit(const camp::Function& function);
    FWDATACAMP_API virtual void visit(const camp::MapProperty& property);
    /**  @} */

    /// Returns the reference object used in comparison.
    FWDATACAMP_API SPTR(::fwData::Object) getReferenceObject() const
    { return m_objRef; }

    /// Returns the compared object.
    FWDATACAMP_API SPTR(::fwData::Object) getComparedObject() const
    { return m_objComp; }

    FWDATACAMP_API const PropsMapType& getReferenceProps() const 
    { return m_propsRef; }

    FWDATACAMP_API const PropsMapType& getComparedProps() const
    { return m_propsComp; }

    FWDATACAMP_API SPTR(PropsMapType) getDifferences()
    { return m_props; }

    /**
     * @brief 
     *
     * @param objRef data object marked as reference object in comparison
     * @param objComp data object marked as compared object to reference object
     *
     * @return 
     */
    FWDATACAMP_API void compare(
            SPTR(::fwData::Object) objRef, SPTR(::fwData::Object) objComp);

private:

    friend struct PropertyVisitor;

    CompareObjects(
            const ::camp::UserObject& obj,
            const std::string& prefix,
            SPTR(PropsMapType) props = SPTR(PropsMapType)(new PropsMapType));

    SPTR(PropsMapType) m_props;

    PropsMapType  m_propsRef;
    PropsMapType  m_propsComp;

    /// Prefix used to identify child objects.
    std::string m_prefix;

    /// Reflection in camp world of currently introspected object
    ::camp::UserObject m_campObj;

    /// Reference object.
    SPTR(::fwData::Object) m_objRef;

    /// Object to be compared with m_objRef.
    SPTR(::fwData::Object) m_objComp;

};

} // namespace visitor

} // namespace fwDataCamp

#endif // __FWDATACAMP_VISITOR_COMPAREOBJECTS_HPP__

