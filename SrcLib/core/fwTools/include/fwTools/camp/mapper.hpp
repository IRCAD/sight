#ifndef __FWTOOLS_CAMP_MAPPER_HPP__
#define __FWTOOLS_CAMP_MAPPER_HPP__

#include <camp/valuemapper.hpp>
#include "fwTools/Type.hpp"
#include "fwTools/BufferObject.hpp"

namespace camp_ext {


template <>
struct ValueMapper< ::fwTools::Type>
{
    typedef ::fwTools::Type ReturnType;
    static const int type = camp::stringType;
    static const std::string to(const ReturnType& source) {return source.string();}

    static ReturnType from(bool source)                    {CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));}
    static ReturnType from(long source)                    {CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));}
    static ReturnType from(double source)                  {CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));}
    static ReturnType from(const std::string& source)      {return ::fwTools::Type::create(source);}
    static ReturnType from(const camp::EnumObject& source) {CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));}
    static ReturnType from(const camp::UserObject& source)
    {
        CAMP_ERROR(camp::BadType(camp::userType, camp::mapType<ReturnType>()));
    }
};


}  // namespace camp_ext

#endif
