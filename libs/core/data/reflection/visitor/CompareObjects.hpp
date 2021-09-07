/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "data/config.hpp"

#include <core/Exception.hpp>
#include <core/macros.hpp>
#include <core/reflection/camp/ExtendedClassVisitor.hpp>
namespace sight::data
{

class Object;
namespace reflection
{

namespace visitor
{

/**
 * @brief   Visitor used to compare two objects.
 */
class DATA_CLASS_API CompareObjects : public ::camp::ExtendedClassVisitor
{
public:

    /// Associates property path (within its parent object) to its value.
    typedef std::map<std::string, std::string> PropsMapType;

    /**
     * @brief Constructor.
     */
    DATA_API CompareObjects();

    DATA_API virtual ~CompareObjects();

    /**
     * @name Introspection methods implementation
     * @{ */
    DATA_API void visit(const camp::SimpleProperty& property);
    DATA_API void visit(const camp::EnumProperty& property);
    DATA_API void visit(const camp::UserProperty& property);
    DATA_API void visit(const camp::ArrayProperty& property);
    DATA_API void visit(const camp::Function& function);
    DATA_API virtual void visit(const camp::MapProperty& property);
    /**  @} */

    /// Returns the reference object.
    SPTR(data::Object) getReferenceObject() const
    {
        return m_objRef;
    }

    /// Returns the compared object.
    SPTR(data::Object) getComparedObject() const
    {
        return m_objComp;
    }

    /// Returns reference object properties.
    const PropsMapType& getReferenceProps() const
    {
        return m_propsRef;
    }

    /// Returns compared object properties.
    const PropsMapType& getComparedProps() const
    {
        return m_propsComp;
    }

    /**
     * @brief Returns the differences found between comparison of reference and compared objects.
     *
     * @return a map using property path within object as key, and associated value found in
     * compared object as value.
     */
    SPTR(PropsMapType) getDifferences() const
    {
        return m_props;
    }

    /**
     * @brief Process to the comparison between reference object and compared object.
     * When the process completes, the differences found between the objects are accessible
     * through ComparedObjects.getDifferences().
     *
     * @param objRef data object marked as reference object in comparison
     * @param objComp data object marked as compared object to reference object
     *
     * @return a map containing the differences found between objects
     *
     * @throw core::Exception if classnames of reference and compared objects are different
     */
    DATA_API void compare(SPTR(data::Object) objRef,
                          SPTR(data::Object) objComp);

    /// Property value for differences map meaning a missing property.
    DATA_API static const std::string s_MISSING_PROPERTY;

private:

    friend struct PropertyVisitor;

    /**
     * @brief Constructor used for child data objects introspection.
     *
     * @param obj child data object representation in Camp world
     * @param prefix path within parent object where then child object is found
     * @param props properties map of parent object to complete with child ones
     */
    CompareObjects(
        const ::camp::UserObject& obj,
        const std::string& prefix,
        SPTR(PropsMapType)props = SPTR(PropsMapType)(new PropsMapType)
    );

    /// Returns the path of the given property within object.
    std::string getPath(const std::string& property) const;

    /// Reference object properties.
    PropsMapType m_propsRef;

    /// Compared object properties.
    PropsMapType m_propsComp;

    /// Reflection in camp world of currently introspected object
    ::camp::UserObject m_campObj;

    /// Prefix used to identify child objects.
    std::string m_prefix;

    /// Differences found between reference and compared object.
    SPTR(PropsMapType) m_props;

    /// Reference object.
    SPTR(data::Object) m_objRef;

    /// Object to be compared with m_objRef.
    SPTR(data::Object) m_objComp;
};

} // namespace visitor

} // namespace reflection

} // namespace sight::data
