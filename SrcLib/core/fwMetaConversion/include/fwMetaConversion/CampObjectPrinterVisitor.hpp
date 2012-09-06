#ifndef  __FWMETACONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__
#define  __FWMETACONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{


class FWMETACONVERSION_CLASS_API CampObjectPrinterVisitor : public ::camp::ExtendedClassVisitor
{

public:

    FWMETACONVERSION_API CampObjectPrinterVisitor( const camp::UserObject & campObj, std::string prefix = std::string() );
    FWMETACONVERSION_API virtual ~CampObjectPrinterVisitor();

    FWMETACONVERSION_API void visit(const camp::SimpleProperty& property);
    FWMETACONVERSION_API void visit(const camp::EnumProperty& property);
    FWMETACONVERSION_API void visit(const camp::UserProperty& property);
    FWMETACONVERSION_API void visit(const camp::ArrayProperty& property);
    FWMETACONVERSION_API void visit(const camp::Function& function);
    FWMETACONVERSION_API virtual void visit(const camp::MapProperty& property);

private :

    std::string getCampType( const ::camp::Type & type );
    const camp::UserObject & m_campObj;
    std::string m_prefix;
};

}

#endif // __FWMETACONVERSION_CAMPOBJECTPRINTERVISITOR_HPP__
