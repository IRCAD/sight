/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    virtual void visit(const Property& property){ClassVisitor::visit(property);};
    virtual void visit(const SimpleProperty& property){ClassVisitor::visit(property);};
    virtual void visit(const ArrayProperty& property){ClassVisitor::visit(property);};
    virtual void visit(const EnumProperty& property){ClassVisitor::visit(property);};
    virtual void visit(const UserProperty& property){ClassVisitor::visit(property);};
    virtual void visit(const Function& function){ClassVisitor::visit(function);};


    FWCAMP_API virtual void visit(const MapProperty& property){}


};


} //camp

#endif /* __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__ */
