/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWMETACONVERSION_CAMPOBJECTVISITOR_HPP__
#define  __FWMETACONVERSION_CAMPOBJECTVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{


class FWMETACONVERSION_CLASS_API CampObjectVisitor : public ::camp::ExtendedClassVisitor
{

public:

    FWMETACONVERSION_API CampObjectVisitor( const camp::UserObject & campObj );
    FWMETACONVERSION_API virtual ~CampObjectVisitor();

    FWMETACONVERSION_API void visit(const camp::SimpleProperty& property);
    FWMETACONVERSION_API void visit(const camp::EnumProperty& property);
    FWMETACONVERSION_API void visit(const camp::UserProperty& property);
    FWMETACONVERSION_API void visit(const camp::ArrayProperty& property);
    FWMETACONVERSION_API void visit(const camp::Function& function);
    FWMETACONVERSION_API virtual void visit(const camp::MapProperty& property);

private :

    std::string getCampType( const ::camp::Type & type );
    const camp::UserObject & m_campObj;
};

}

#endif // __FWMETACONVERSION_CAMPOBJECTVISITOR_HPP__
