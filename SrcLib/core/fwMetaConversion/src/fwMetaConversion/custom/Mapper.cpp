#include <fwMetaData/MetaVisitor.hpp>

#include "fwMetaConversion/custom/Mapper.hpp"
#include "fwMetaConversion/DataVisitor.hpp"
#include "fwMetaConversion/policy/Data.hpp"

namespace fwMetaConversion
{
namespace custom
{



::fwMetaData::Object::sptr
Mapper::computeMinimalInfos(::fwData::Object::sptr object,
                            ::fwMetaConversion::MetaHelper& metaHelper)
{
    const camp::Class& metaclass = camp::classByName(object->getClassname());
    ::fwMetaConversion::DataVisitor visitor(object, metaHelper);

    metaclass.visit(visitor);

    return visitor.getMetaObject();
}

::fwData::Object::sptr
Mapper::computeMinimalInfos(::fwMetaData::Object::sptr object,
                            ::fwMetaConversion::MetaHelper& metaHelper)
{

    const camp::Class& metaclass = camp::classByName(object->getClassname());
    ::fwMetaConversion::policy::Data policy(metaHelper);
    ::fwMetaData::MetaVisitor visitor(policy, object);
    metaclass.visit(visitor);

    return policy.getObject();
}

}
}
