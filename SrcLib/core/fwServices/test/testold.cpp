/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/shared_ptr.hpp>
//using namespace boost;


//#include "fwTools/Object.hpp"
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwCore/base.hpp>
//#include "fwServices/Consultable.hpp"
//#include "fwServices/action/PointerConsult.hpp"
//#include "fwServices/action/ClassNameConsult.hpp"
//#include "fwServices/Consultable.hpp"
//#include "fwServices/action/ActionFactoryRegistry.hpp"
//#include "fwServices/action/ActionRegistrar.hpp"


//template< typename CONTAINER>
//void affiche(CONTAINER &c)
//{
//  typename CONTAINER::iterator i = c.begin();
//  std::cout << "[";
//  for ( ; i != c.end() ; ++i )
//  {
//      std::cout << " " << *i;
//  }
//  std::cout << " ]";
//}
//
//
//// declare data example
//namespace fwData
//{
//
//  struct ObjectA : public Object
//  {
//      ObjectA(): Object( "ObjectA" ) {}
//  };
//
//  struct ObjectB : public Object
//  {
//      ObjectB(): Object( "ObjectB" ) {}
//  };
//
//  struct ObjectB1 : public ObjectB
//  {
//      ObjectB1()  { className() =  "ObjectB1"; }
//  };
//
//}
//
//
//
//// show difference between typeid & explicit naming
//void TypeIdBehavior()
//{
//  
//  fwTools::ObjectA  oa;
//  fwTools::ObjectA  oaa;
//  fwTools::ObjectB  ob;
//  fwTools::ObjectB1 ob1;
//  
//  fwTools::Object *o = &oa;
//  
//  std::cout << "typeid(&oa).name()" << typeid(&oa).name() << "\n";
//  std::cout << "typeid(&ob).name()" << typeid(&ob).name() << "\n";
//  std::cout << "typeid(o).name()" << typeid(o).name() << "\n";
//  
//  std::cout << "oa.getClassName()" << oa.className() << "\n";
//  std::cout << "ob.getClassName()" << ob.className() << "\n";
//  std::cout << "o->getClassName()" << o->className() << "\n";
//  
//  
//  
//}
//
////::fwServices::action::ActionRegistrar< action::IConsult, action::ClassNameConsult , fwTools::ObjectB > registrar
////REGISTER_ACTION( ::fwServices::action::IConsult, ::fwServices::action::ClassNameConsult , fwTools::ObjectB );
//
//
//void testFactoryRegistration()
//{
//  using namespace fwServices;
//  
////    //  for action developer an helper will be created 
////    typedef action::ActionFactory< action::IConsult, action::PointerConsult, fwTools::ObjectA >  ActionFactoryPointerConsultObjectA;
////     ::boost::shared_ptr< action::IActionFactory > af( new ActionFactoryPointerConsultObjectA );
////    
////    action::ActionFactoryRegistry::addFactory( af );
////    
////    
////     ::boost::shared_ptr<fwTools::ObjectA>  oa( new fwTools::ObjectA() );
////     ::boost::shared_ptr<fwTools::ObjectA> oaa( new fwTools::ObjectA() );
////     ::boost::shared_ptr<fwTools::ObjectB>  ob( new fwTools::ObjectB() );
////    
////     ::boost::shared_ptr<fwServices::Consultable> consultService( new fwServices::Consultable()  );
////    // note here we simulate a singleton by using same addresse of consultService
////    
////    
////    //create & attach the service
////    ObjectServiceRegistry::getDefault()->registerService( oa, consultService );
////    
////    // get Service
////     ::boost::shared_ptr< Consultable > s = 
////    ObjectServiceRegistry::getDefault()->getServices< Consultable >( oa ).front();
////    assert( s.get() );
////    
////    // get the action and execute it
////     ::boost::shared_ptr< action::IAction > action = s->consultAction( oa.get());
////    assert( action.get() ); 
////    action->execute( oa.get() );
////    
////    // oaa have no Consultable Service
////    assert ( ObjectServiceRegistry::getDefault()->getServices< Consultable >( oaa ).empty() );
////    /// ob have no Consultable Service
////    assert ( ObjectServiceRegistry::getDefault()->getServices< Consultable >( ob  ).empty() );
////    
////    ////////////////////////////////////////////////////////////////////////////////////////////
////    // register consultService service for ob
////    ObjectServiceRegistry::getDefault()->registerService( ob, consultService );
////    
////    // get Service
////    s = ObjectServiceRegistry::getDefault()->getServices< Consultable >( ob ).front();
////    assert( s.get() );
////    
////    // service is here but no action can deal ObjectB type
////    assert ( s->consultAction( ob.get()).get() == 0 );
////    
////    
////    // register a new Action that can deal ObjectB type
////    //typedef action::ActionFactory< action::IConsult, action::ClassNameConsult, fwTools::ObjectB >  ActionFactoryclassNameConsultObjectB;
////    // ::boost::shared_ptr< action::IActionFactory > af2( new ActionFactoryclassNameConsultObjectB );
////    //action::ActionFactoryRegistry::addFactory( af2 );
////    // a helper is provided : REGISTER_ACTION( action::IConsult, action::ClassNameConsult , fwTools::ObjectB );
////    //::fwServices::action::ActionRegistrar< fwServices::action::IConsult, fwServices::action::ClassNameConsult , fwTools::ObjectB > registrar; // OK !
////    action::ActionRegistrar< action::IConsult, action::ClassNameConsult , fwTools::ObjectB > registrar; // OK !
////
////    //registrar;
////
////    // now service can produce an action
////    action = s->consultAction( ob.get());
////    assert( action.get() ); 
////    action->execute( ob.get() ); // call ClassNameConsult
//  
//  
//  
//
//  
//  
//  
//}




int main(int argc, char **argv)
{
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > config(new ::fwRuntime::EConfigurationElement("rootElement"));
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > input = config->addConfigurationElement("subElement");
    input->setAttributeValue("myAttr","myValue");
    OSLM_INFO("configuration element = " << *config) ;
//  TypeIdBehavior();
//
//      testFactoryRegistration();
    
    return 0;
}
