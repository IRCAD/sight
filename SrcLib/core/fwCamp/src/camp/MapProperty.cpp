#include "fwCamp/camp/customtype.hpp"
#include "fwCamp/camp/MapProperty.hpp"
#include "fwCamp/camp/ExtendedClassVisitor.hpp"

namespace camp {





MapProperty::MapProperty(const std::string& name, Type elementType):
            Property(name, (::camp::Type) mappingType),
            m_elementType(elementType)
{

}

MapProperty::~MapProperty()
{
}

void MapProperty::accept(ClassVisitor& visitor) const
{
    ExtendedClassVisitor* mapVisitor = dynamic_cast< ExtendedClassVisitor*>(&visitor);
    if (mapVisitor)
    {
        mapVisitor->visit(*this);
    }
    else
    {
        visitor.visit(*this);
    }
}

Value MapProperty::getValue(const UserObject& object) const
{
    return getElement(object, 0);
}


void MapProperty::setValue(const UserObject& object, const Value& value) const
{
    //FIXME XXX TODO DO somethings
}

Type MapProperty::elementType() const
{
    return m_elementType;
}

}  // namespace camp
