/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_CAMPOBJECTVISITOR_HPP__
#define  __FWATOMCONVERSION_CAMPOBJECTVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{


class FWATOMCONVERSION_CLASS_API CampObjectVisitor : public ::camp::ExtendedClassVisitor
{

public:

    FWATOMCONVERSION_API CampObjectVisitor( const camp::UserObject & campObj );
    FWATOMCONVERSION_API virtual ~CampObjectVisitor();

    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);
    FWATOMCONVERSION_API void visit(const camp::Function& function);
    FWATOMCONVERSION_API virtual void visit(const camp::MapProperty& property);

private :

    std::string getCampType( const ::camp::Type & type );
    const camp::UserObject & m_campObj;
};

}

#endif // __FWATOMCONVERSION_CAMPOBJECTVISITOR_HPP__
