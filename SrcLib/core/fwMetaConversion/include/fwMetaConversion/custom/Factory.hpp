#ifndef __FWMETACONVERSION_CUSTOM_FACTORY_HPP__
#define __FWMETACONVERSION_CUSTOM_FACTORY_HPP__


#include <map>
#include <string>

#include <boost/preprocessor/seq/fold_left.hpp>

#include "fwMetaConversion/custom/Mapper.hpp"
#include "fwMetaConversion/custom/Factory.hpp"

#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{
namespace custom
{

class FWMETACONVERSION_CLASS_API ObjectFactory {
public:
    std::map<std::string, Mapper*> creators_factory;

    FWMETACONVERSION_API static ObjectFactory* instance();
    FWMETACONVERSION_API bool reg(const std::string& id, Mapper* mapper);
    FWMETACONVERSION_API static Mapper* get(const std::string& id);

private:
    static ObjectFactory* s_self;
};

#define fwMetaConversionRegisterMacro(T, I )\
::fwMetaConversion::custom::ObjectFactory::instance()->reg(#T, new I())

}
}

#endif /* __FWMETACONVERSION_CUSTOM_FACTORY_HPP__ */
