/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/reflection/camp/MapProperty.hpp"

#include <camp/classvisitor.hpp>

namespace camp
{

class CORE_CLASS_API ExtendedClassVisitor : public ClassVisitor
{
public:

    //------------------------------------------------------------------------------

    virtual void visit(const Property& property)
    {
        ClassVisitor::visit(property);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const SimpleProperty& property)
    {
        ClassVisitor::visit(property);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const ArrayProperty& property)
    {
        ClassVisitor::visit(property);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const EnumProperty& property)
    {
        ClassVisitor::visit(property);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const UserProperty& property)
    {
        ClassVisitor::visit(property);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const Function& function)
    {
        ClassVisitor::visit(function);
    }
    //------------------------------------------------------------------------------

    virtual void visit(const MapProperty&)
    {
    }
};

} //camp
