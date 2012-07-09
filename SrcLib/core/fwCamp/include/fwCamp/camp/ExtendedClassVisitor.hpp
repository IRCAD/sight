#ifndef __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__
#define __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__

#include <camp/classvisitor.hpp>
#include "fwCamp/camp/MapProperty.hpp"
#include "fwCamp/config.hpp"

namespace camp
{



class FWCAMP_CLASS_API ExtendedClassVisitor : public ClassVisitor
{
public:
    FWCAMP_API virtual void visit(const MapProperty& property){}

};


} //camp

#endif /* __FWCAMP_CAMP_EXTENDEDCLASSVISITOR_HPP__ */
