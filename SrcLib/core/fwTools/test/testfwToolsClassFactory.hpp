/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/ClassRegistrar.hpp"

struct Base
{};

struct Sub : public Base
{};




struct FactoryWeakOrderer
{
  bool operator()( ::boost::shared_ptr< ::fwTools::IClassFactory> f1, ::boost::shared_ptr< ::fwTools::IClassFactory> f2) const
  {
	  std::cout << "FactoryWeakOrderer :" << std::endl;
//	  std::cout << "f1= " << f1 << " f1.px " << f1.get() <<  " f2= " << f2 << " f2.px " << f2.get();
//	  std::cout << " (*f1)==(*f2)=" <<  ((*f1)==(*f2)) << "  f1 < f2=" << (f1 < f2) << std::endl;;
//	 return !( (*f1)==(*f2) ) && (f1 < f2); // factory which produce same things (but can be different by ptr)
//										  //are considerer to be equal
	  return (*f1) < (*f2);

  }
};

void testRegistrationUnicity()
{
	static int val1=1;
	static int val1bis=1;
	static int val2=2;

	 ::boost::shared_ptr< ::fwTools::IClassFactory >  f1( new ::fwTools::ClassFactory< Base,Sub,int >(val1) );
	 ::boost::shared_ptr< ::fwTools::IClassFactory >  ff1( new ::fwTools::ClassFactory< Base,Sub,int >(val1bis) );

	 ::boost::shared_ptr< ::fwTools::IClassFactory >  f2( new ::fwTools::ClassFactory< Base,Sub,int >(val2) );

	 ::boost::shared_ptr< ::fwTools::IClassFactory >  g( new ::fwTools::ClassFactory< Base,Base,int >(val1) );

	std::cout << "(*f1)==(*ff1)=" <<  ((*f1)==(*ff1)) << std::endl;
	std::cout << "(*f1)==(*f2)=" <<  ((*f1)==(*f2)) << std::endl;
	std::cout << "(*f1)==(*g)=" <<  ((*f1)==(*g)) << std::endl;



	typedef std::set< ::boost::shared_ptr< ::fwTools::IClassFactory> ,FactoryWeakOrderer > FactoryContainer;
	FactoryContainer ss;

	std::cout << "f1= "  << f1<< std::endl;
	std::cout << "ff1= " << ff1<< std::endl;
	std::cout << "f2= "  << f2<< std::endl;
	std::cout << "g= "   << g<< std::endl;
	ss.insert(f1);
	ss.insert(f1);
	ss.insert(ff1);
	ss.insert(f2);
	std::cout << "size=" << ss.size();


	std::cout << "TYPEINFOTEST";
	 ::boost::shared_ptr< ::fwTools::IClassFactory >  t1( new ::fwTools::ClassFactory< Base,Sub, std::type_info >( typeid(int) ));
	 ::boost::shared_ptr< ::fwTools::IClassFactory >  tt1( new ::fwTools::ClassFactory< Base,Sub, std::type_info >( typeid(int) ));

	 ::boost::shared_ptr< ::fwTools::IClassFactory >  t2( new ::fwTools::ClassFactory< Base,Sub, std::type_info >(typeid(float) ));
	 ::boost::shared_ptr< ::fwTools::IClassFactory >  s( new ::fwTools::ClassFactory< Base,Base,std::type_info >( typeid(int) ));

	std::cout << "(*t1)==(*tt1)=" <<  ((*t1)==(*tt1)) << std::endl;
	std::cout << "(*t1)==(*t2)=" <<  ((*t1)==(*t2)) << std::endl;
	std::cout << "(*t1)==(*s)=" <<  ((*t1)==(*s)) << std::endl;

	std::cout << "find operator";
	std::cout << (ss.find(ff1) != ss.end());


}
