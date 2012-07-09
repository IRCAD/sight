#ifndef __FWCAMP_USEROBJECT_HPP__
#define __FWCAMP_USEROBJECT_HPP__

#include <camp/userobject.hpp>

#include <fwCore/macros.hpp>

#include "fwCamp/Factory.hpp"

fwCorePredeclare((fwTools)(Object));


namespace fwCamp
{

typedef Factory< ::camp::UserObject * (*)( ::fwTools::Object * ) > UserObjectFactory;


/**
 * @brief Helper to register and manage the conversion between an fwData::Object and a camp::userObject
 */
template<typename T>
struct UserObjectRegistrar
{
    /**
     * @brief Register an object of type name in factory.
     */
    UserObjectRegistrar( const std::string &name );

    /**
     * @brief create a camp::UserObject from a fwTools::Object
     */
    static ::camp::UserObject *create( ::fwTools::Object *object );

};

}

#include "fwCamp/UserObject.hxx"
#endif
