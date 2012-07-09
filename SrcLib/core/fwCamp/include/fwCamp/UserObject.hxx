
#ifndef __FWCAMP_USEROBJECT_HXX__
#define __FWCAMP_USEROBJECT_HXX__


namespace fwCamp {

template<typename T>
UserObjectRegistrar<T>::UserObjectRegistrar( const std::string &name )
{
    UserObjectFactory::Registrar reg1(name, &create);
}

template<typename T>
::camp::UserObject* UserObjectRegistrar<T>::create( ::fwTools::Object *object )
{
    return new ::camp::UserObject( *(dynamic_cast<T*>( object )) );
}

}

#endif
