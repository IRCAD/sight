#include "fwMetaConversion/custom/Factory.hpp"

namespace fwMetaConversion
{
namespace custom
{

ObjectFactory* ObjectFactory::s_self = NULL;

ObjectFactory* ObjectFactory::instance()
{
    if (s_self == NULL)
    {
        s_self = new ObjectFactory();
    }

    return s_self;

}

bool ObjectFactory::reg(const std::string& id, Mapper* mapper)
{
    creators_factory[id] = mapper;
    return true;
}

// pass in creator function pointer to register it to id
Mapper* ObjectFactory::get(const std::string& id) {
    return instance()->creators_factory[id];
}


}
}
