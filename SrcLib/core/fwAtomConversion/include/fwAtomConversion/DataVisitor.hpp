/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_DATAVISITOR_HPP__
#define  __FWATOMCONVERSION_DATAVISITOR_HPP__

#include <string>
#include <map>


#include <fwTools/UUID.hpp>
#include <fwCamp/camp/ExtendedClassVisitor.hpp>
#include <fwData/Object.hpp>
#include <fwAtoms/Base.hpp>

#include "fwAtomConversion/AtomHelper.hpp"
#include "fwAtomConversion/config.hpp"


namespace fwAtomConversion
{


class FWATOMCONVERSION_CLASS_API DataVisitor : public ::camp::ExtendedClassVisitor
{
public:
    typedef ::fwAtoms::Object::Attributes Attributs;
    typedef std::map< ::fwTools::UUID::UUIDType, ::fwAtoms::Base::sptr> References;


    FWATOMCONVERSION_API DataVisitor(::fwData::Object::sptr data, ::fwAtomConversion::AtomHelper& helper);
    FWATOMCONVERSION_API virtual ~DataVisitor();

    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);
    FWATOMCONVERSION_API void visit(const camp::Function& function);
    FWATOMCONVERSION_API virtual void visit(const camp::MapProperty& property);

    FWATOMCONVERSION_API ::fwAtoms::Base::sptr matchCampType(camp::Type type, const camp::Value& value);

    FWATOMCONVERSION_API ::fwAtoms::Object::sptr getAtomObject();

private:

    ::fwAtoms::Base::sptr processObject(const ::camp::Value& value);

    CSPTR(camp::UserObject) m_obj;
    ::fwAtomConversion::AtomHelper& m_helper;


    ::fwAtoms::Object::sptr m_metaObject;

    References m_refMap;
};

}

#endif /* __FWATOMCONVERSION_DATAVISITOR_HPP__ */
