/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__
#define  __FWATOMCONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{


class FWATOMCONVERSION_CLASS_API CampObjectPrinterVisitor : public ::camp::ExtendedClassVisitor
{

public:

    FWATOMCONVERSION_API CampObjectPrinterVisitor( const camp::UserObject & campObj, std::string prefix = std::string() );
    FWATOMCONVERSION_API virtual ~CampObjectPrinterVisitor();

    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);
    FWATOMCONVERSION_API void visit(const camp::Function& function);
    FWATOMCONVERSION_API virtual void visit(const camp::MapProperty& property);

private :

    std::string getCampType( const ::camp::Type & type );
    const camp::UserObject & m_campObj;
    std::string m_prefix;
};

}

#endif // __FWATOMCONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__
