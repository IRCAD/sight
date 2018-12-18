/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__
#define __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__

#include <camp/classvisitor.hpp>
#include "fwCamp/camp/MapProperty.hpp"
#include "fwCamp/config.hpp"

namespace camp
{

class FWCAMP_CLASS_API ExtendedClassVisitor : public ClassVisitor
{
public:

    virtual void visit(const Property& property)
    {
        ClassVisitor::visit(property);
    }
    virtual void visit(const SimpleProperty& property)
    {
        ClassVisitor::visit(property);
    }
    virtual void visit(const ArrayProperty& property)
    {
        ClassVisitor::visit(property);
    }
    virtual void visit(const EnumProperty& property)
    {
        ClassVisitor::visit(property);
    }
    virtual void visit(const UserProperty& property)
    {
        ClassVisitor::visit(property);
    }
    virtual void visit(const Function& function)
    {
        ClassVisitor::visit(function);
    }


    FWCAMP_API virtual void visit(const MapProperty& property)
    {
    }


};


} //camp

#endif /* __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__ */
