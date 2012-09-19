/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWMETACONVERSION_DATAVISITOR_HPP__
#define  __FWMETACONVERSION_DATAVISITOR_HPP__

#include <string>
#include <map>


#include <fwTools/UUID.hpp>
#include <fwCamp/camp/ExtendedClassVisitor.hpp>
#include <fwData/Object.hpp>
#include <fwMetaData/Base.hpp>

#include "fwMetaConversion/MetaHelper.hpp"
#include "fwMetaConversion/config.hpp"


namespace fwMetaConversion
{


class FWMETACONVERSION_CLASS_API DataVisitor : public ::camp::ExtendedClassVisitor
{
public:
    typedef ::fwMetaData::Object::Attributes Attributs;
    typedef std::map< ::fwTools::UUID::UUIDType, ::fwMetaData::Base::sptr> References;


    FWMETACONVERSION_API DataVisitor(::fwData::Object::sptr data, ::fwMetaConversion::MetaHelper& helper);
    FWMETACONVERSION_API virtual ~DataVisitor();

    FWMETACONVERSION_API void visit(const camp::SimpleProperty& property);
    FWMETACONVERSION_API void visit(const camp::EnumProperty& property);
    FWMETACONVERSION_API void visit(const camp::UserProperty& property);
    FWMETACONVERSION_API void visit(const camp::ArrayProperty& property);
    FWMETACONVERSION_API void visit(const camp::Function& function);
    FWMETACONVERSION_API virtual void visit(const camp::MapProperty& property);

    FWMETACONVERSION_API ::fwMetaData::Base::sptr matchCampType(camp::Type type, const camp::Value& value);

    FWMETACONVERSION_API ::fwMetaData::Object::sptr getMetaObject();

private:

    ::fwMetaData::Base::sptr processObject(const ::camp::Value& value);

    const camp::UserObject* m_obj;
    ::fwMetaConversion::MetaHelper& m_helper;


    ::fwMetaData::Object::sptr m_metaObject;

    References m_refMap;
};

}

#endif /* __FWMETACONVERSION_DATAVISITOR_HPP__ */
