/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/mpl/vector.hpp>

#include <boost/mpl/size.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/equal.hpp>
#include <iostream>
#include <list>
#include <vector>

#include "fwTools/IntrinsicTypes.hpp"

#include "fwTools/Dispatcher.hpp"

#include "fwTools/StringKeyTypeMapping.hpp"
#include "fwTools/TypeInfoKeyTypeMapping.hpp"
#include "fwTools/Combinatory.hpp"

#include "fwTools/Singleton.hpp"
#include "fwTools/DynamicType.hpp"

#include "fwTools/Stringizer.hpp"

#include "fwTools/UUID.hpp"
#include <utility>
#include "../test/testfwToolsClassFactory.hpp"


//static bool a = fwTools::ClassName::registration<double>("float");
//static bool a = fwTools::ClassName::registration<double>(BOOST_PP_STRINGIZE(float));


class TestSingleton : public fwTools::Singleton< TestSingleton >
{
	friend class  fwTools::Singleton< TestSingleton >; // mandatory iff default Ctor protected

public :
protected :
	TestSingleton(){};

	int m_int;
};





struct PrintTypeInfo
{
	template<class T>
	void operator()()
	{
		std::cout << typeid(T).name() << " ";
	}
};




struct PrintTypeInfo2
{
	template<class T1,class T2>
	void operator()(void)
	{
		std::cout << "{" << typeid(T1).name() << " , " << typeid(T2).name() << "}";
	}
};








struct PrintTypeInfo2Wrapper
{
	template<class TSEQ>
	void operator()()
	{
		typedef  typename boost::mpl::at_c<TSEQ, 0 >::type T1;
		typedef  typename boost::mpl::at_c<TSEQ, 1 >::type T2;
		PrintTypeInfo2 p; // use with two lines else win32 cl7-1 fail to compile
		p.operator()<T1,T2>();
	}

};






struct PrintTypeInfoTrio
{
	template<class TSEQ>
	void operator()()
	{
		BOOST_STATIC_ASSERT ( boost::mpl::is_sequence<TSEQ>::value );

		// demonstrate how to obtain element from typelist
		typedef typename boost::mpl::at_c<TSEQ, 0 >::type FirstType;
		typedef typename boost::mpl::at_c<TSEQ, 1 >::type SecondType;
		typedef typename boost::mpl::at_c<TSEQ, 2 >::type ThridType;

		std::cout << "{" << typeid(FirstType).name() << " , " << typeid(SecondType).name() << " , " << typeid(ThridType).name() << "}";

	}
};





struct PrintSizeOf
{
	template<class T>
	void operator()()
	{
		std::cout << sizeof(T) << " ";
	}
};




struct PrintTypeInfoDelimitedTemplateVersion
{

	template<class T>
	void operator()(const std::string &delimiter)
	{
		std::cout << delimiter<< typeid(T).name() << delimiter;
		// if delimiter is not const then it could be modified here !!!!
	}
};





struct BaseClass
{
	std::string m_data;
};


template <class T>
struct SubClass : public BaseClass
{
	SubClass()
	{
		m_data=typeid(T).name();
	}
};


struct SubClassFactory
{
	template<class T>
	void operator()( BaseClass  *  & bclass) // reference on a pointer
	{
		bclass = (new SubClass<T>() );
	}
};





void testSimpleOK()
{
	using namespace fwTools;

	// simple test instanciate + call all method dor different functor
	typedef boost::mpl::vector<unsigned char, signed char, signed short, unsigned short, float >::type  SupportedTypes;

	Dispatcher<SupportedTypes,PrintTypeInfo>::invoke();
	Dispatcher<SupportedTypes,PrintSizeOf>::invoke();

	std::string wantedtype("float");
	Dispatcher<SupportedTypes,PrintTypeInfo>::invoke(wantedtype);

	std::string wantedtype2("signed short");
	// show how to use parameter
	std::string delimiter("----");
	Dispatcher<SupportedTypes,PrintTypeInfoDelimitedTemplateVersion>::invoke(wantedtype2,delimiter);

	BaseClass *baseClass=NULL;
	Dispatcher< SupportedTypes , SubClassFactory >::invoke(wantedtype2, baseClass);
	assert ( baseClass!=NULL );
	std::cout << "baseClass=" << baseClass->m_data  << std::endl;


}




void testBinaryCartesianProductOK()
{
	using namespace fwTools;
	using namespace boost::mpl;

	typedef vector< signed char, signed short, signed int>::type Set1;
	typedef vector< float, double >::type Set2;

	typedef vector< vector< signed char,float>, vector< signed char,double>,
					vector< signed short,float>, vector< signed  short,double>,
					vector< signed int,float>, vector< signed int,double>
				  > Wanted;



	typedef apply< BinaryCartesianProduct, Set1, Set2 >::type Result;

	BOOST_MPL_ASSERT_RELATION( size< Result >::value , == , 6 );
    BOOST_MPL_ASSERT_NOT(( boost::is_same< front<Result>::type, vector<float> >  )); // is same failure :/ vector1<> != vector ...

    // BOOST_MPL_ASSERT(( equal< Result, Wanted > )); equal is not container recursive :/
   	BOOST_MPL_ASSERT(( equal< front<Result>::type, vector2< signed char, float >  > ));
    BOOST_MPL_ASSERT(( equal< at_c<Result,1>::type, vector2<signed char, double> > ));
    BOOST_MPL_ASSERT(( equal< at_c<Result,2>::type, vector2<signed short,float> > ));
    BOOST_MPL_ASSERT(( equal< at_c<Result,3>::type, vector2<signed short,double> > ));
    BOOST_MPL_ASSERT(( equal< at_c<Result,4>::type, vector2<signed int,float> > ));
    BOOST_MPL_ASSERT(( equal< back<Result>::type, vector2<signed int,double> >   ));

   BOOST_MPL_ASSERT_NOT(( equal< back<Result>::type, vector2<int,int> >   ));
   BOOST_MPL_ASSERT_NOT(( equal< back<Result>::type, vector<int> >   ));

   typedef std::list<std::string> KeyTypeContainer;
	KeyTypeContainer keys;
	keys.push_back("signed short");keys.push_back("float");

	Dispatcher<Result,PrintTypeInfo2Wrapper>::invoke(keys);

}




void testBinaryCartesianProductLimitCaseOK()
{
	using namespace fwTools;
	using namespace boost::mpl;
	typedef vector<> emptySet;
	typedef vector< char, short>  Set1;

	typedef vector< vector<char>, vector<short> >::type Wanted;

	typedef apply< BinaryCartesianProduct, Set1, emptySet >::type  Result;
	BOOST_MPL_ASSERT_RELATION( size<Wanted>::value , == , size<Result>::value  );
	BOOST_MPL_ASSERT(( equal< front<Wanted> , front<Result> > ));
	BOOST_MPL_ASSERT(( equal< at_c<Wanted,0> , at_c<Result,0> > ));
	BOOST_MPL_ASSERT(( equal< at_c<Wanted,1> , at_c<Result,1> > ));
}



void testCartesianProductOK()
{
	using namespace fwTools;
	using namespace boost::mpl;

	typedef vector< vector<signed char, signed short, signed int>,  vector< float, double >  , vector< std::string, unsigned char >   > ::type MultiSet;

	typedef apply< CartesianProduct, MultiSet>::type Result;

	BOOST_MPL_ASSERT_RELATION( size< Result >::value , == , 12 );

	BOOST_MPL_ASSERT(( equal<  at_c<Result,0>::type, vector<  signed char,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,1>::type, vector<  signed char,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,2>::type, vector<  signed char,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,3>::type, vector<  signed char,  double , unsigned char >    > ));

	BOOST_MPL_ASSERT(( equal<  at_c<Result,4>::type, vector< signed short,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,5>::type, vector< signed short,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,6>::type, vector< signed short,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,7>::type, vector< signed short,  double , unsigned char >    > ));

	BOOST_MPL_ASSERT(( equal<  at_c<Result,8>::type, vector<  signed int,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,9>::type, vector<  signed int,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,10>::type, vector< signed int,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,11>::type, vector< signed int,  double , unsigned char >    > ));


	typedef std::list<std::string> KeyTypeContainer;
	KeyTypeContainer keys;
	keys.push_back("signed short");keys.push_back("float");keys.push_back("std::string");

	Dispatcher<Result,PrintTypeInfoTrio>::invoke(keys);

}



int testDynamicType()
{
	using namespace fwTools;
	DynamicType d;
	std::cout << "d.string() => " << d.string() << std::endl ;

	d.setType<float>();
	std::cout << "d.setType<float>();d.string() => " << d.string() << std::endl ;

	std::cout << "d.isType<int>() => " << d.isType<int>() << std::endl ;
	std::cout << "d.isType<float>() => " << d.isType<float>() << std::endl ;

	try
	{
	d.setType<std::string>();
	}
	catch ( std::exception &e )
	{
		std::cout << "OK catched [" << e.what() << "]" << std::endl;
	}


	DynamicType::registerNewType< std::string >("std::string");
	d.setType<std::string>();
	std::cout << "d.setType<std::string>();d.string() => " << d.string() << std::endl ;


	try
	{
		DynamicType::registerNewType< float >("float");
	}
	catch ( std::exception &e )
	{
		std::cout << "OK catched [" << e.what() << "]" << std::endl;
	}

	try
	{
			DynamicType::registerNewType< float >("floattttteeee");
	}
	catch ( std::exception &e )
	{
		std::cout << "OK catched [" << e.what() << "]" << std::endl;
	}


	makeDynamicType<unsigned char>();

	std::cout << "makeDynamicType(typeid(int)).string()" << makeDynamicType(typeid(int)).string() << std::endl;

	try
	{
		std::cout << "makeDynamicType( std::string(toto) ).string()" << makeDynamicType(  std::string("toto") ).string() << std::endl;
	}
	catch ( std::exception &e )
	{
			std::cout << "OK catched [" << e.what() << "]" << std::endl;
	}


	return 0;
}


void testTypeInfoMapping()
{
	using namespace fwTools;
	std::cout << "isMapping<signed int>( typeid(signed int)) =" << isMapping<signed int>( typeid(signed int)) << std::endl;
	assert( isMapping<signed int>( typeid(signed int)) );
	std::cout << "isMapping< int      >( typeid(signed int)) =" << isMapping< int      >( typeid(signed int)) << std::endl;
	assert( isMapping<signed int>( typeid(signed int)) );
	std::cout << "isMapping< float    >( typeid(signed int)) =" << isMapping< float    >( typeid(signed int)) << std::endl;
	assert( isMapping< float    >( typeid(signed int)) == false  );
}



struct Delimitator
{
	template< class T >
	void operator()( std::string & delim )
	{
		std::cout << delim << typeid(T).name() << delim << std::endl;
		delim="MODIF";
	}
};


void testDispatcherFunctor()
{
	using namespace fwTools;
	std::string delimiter="----";
	Dispatcher< IntrinsicTypes, Delimitator >::invoke( std::string("float"), delimiter );

	std::cout << "delimiter" << delimiter << std::endl;
}



struct ClassA { };
void testClassName()
{

	using namespace fwTools;
	//ClassName record;
	std::cout <<  "ClassName::isRegistred<double>()" <<  ClassName::isRegistred<double>() << std::endl;
	std::cout <<  "ClassName::isRegistred<int>()" <<  ClassName::isRegistred<int>() << std::endl;
	std::cout <<  "ClassName::isRegistred(""int"")" <<  ClassName::isRegistred("int") << std::endl;
	std::cout <<  "ClassName::isRegistred(""toto"")" <<  ClassName::isRegistred("toto") << std::endl;
	std::cout << "REGSITRING INT" << std::endl;
	ClassName::registration< int >("int");
	std::cout <<  "ClassName::isRegistred<double>()" <<  ClassName::isRegistred<double>() << std::endl;
	std::cout <<  "ClassName::isRegistred<int>()" <<  ClassName::isRegistred<int>() << std::endl;
	std::cout <<  "ClassName::isRegistred(""int"")" <<  ClassName::isRegistred("int") << std::endl;
	std::cout <<  "ClassName::isRegistred(""toto"")" <<  ClassName::isRegistred("toto") << std::endl;

	std::string name("classA");
	ClassName::registration< ClassA >("classA");

	ClassA a;
	std::cout << "ClassName::get(typeid(a))" << ClassName::get(typeid(a)) << std::endl;;

	std::cout << "ClassName::get(""classA"")" << ClassName::get("classA").name() << std::endl;;
	std::cout << "ClassName::get(""JamaisDef"")" << ClassName::get("JamaisDef").name() << std::endl;;
	std::cout << "ClassName::get(""int"")" << ClassName::get("int").name() << std::endl;;

}


void testTypeID()
{
	float f;
	float *pf;
	const float *pcf;
	float const *pfc;

#define showVINCE(x) std::cout << "typeinfo of " << #x << "=" << typeid(x).name() << std::endl;

	showVINCE(int);
	showVINCE(f);
	showVINCE(*pcf);
	showVINCE(*pfc);
	showVINCE(const float);
	showVINCE(float const);

	//std::cout << typeid(float) << "

}

void testUUID()
{
	using namespace fwTools;

	ClassName::registration< int >( std::string("INT") );
	ClassName::registration< float >( std::string("FLOAT") );


//	std::cout << UUID::.generate(typeid(int));
//	std::cout << UUID::.generate(typeid(int));
//	std::cout << UUID::.generate(typeid(float));
//
//	std::cout << UUID::.generate(typeid(float));
//	std::cout << UUID::.generate(typeid(int));

	std::cout << "-----\n";

	::boost::shared_ptr<int> i1(new int);
	::boost::shared_ptr<float> s2(new float);
	::boost::shared_ptr<float> s3(new float);

	::boost::weak_ptr<int> i11 = i1;
	::boost::weak_ptr<float> s22 = s2;;

	std::cout << UUID::get(i11)<< std::endl;
	std::cout << UUID::get(i11)<< std::endl;
	std::cout << UUID::get(s2)<< std::endl;
	std::cout << UUID::get(s3)<< std::endl;
	std::cout << UUID::get(s2)<< std::endl;
	std::cout << UUID::get(i11)<< std::endl;
	std::cout << UUID::get(s3)<< std::endl;

	//UUID::impose(s3,"toto"); //OK will throw an exception UUID::impose failed object have a previous uuid old=FLOAT-1 imposed=toto

	::boost::shared_ptr<float> stest(new float);
	 //UUID::impose(s4,"FLOAT-1"); // OK exception what():  UUID::impose failed uuid [FLOAT-1] already exist

	UUID::impose(stest,"FLOAT-2", UUID::SIMPLE ); // OK
	boost::shared_ptr<float> s5(new float);
	std::cout << UUID::get(s5) << std::endl; // OK will skip FLOAT-2 and generate float-3


	// no the same but with extended
	std::cout << std::endl;
	std::cout << UUID::get(i11,UUID::RFC4122) << std::endl;
	std::cout << UUID::get(i11,UUID::EXTENDED)<< std::endl;
	std::cout << UUID::get(s22,UUID::RFC4122)<< std::endl;
	std::cout << UUID::get(s3,UUID::RFC4122)<< std::endl;
	std::cout << UUID::get(s2,UUID::RFC4122)<< std::endl;
	std::cout << UUID::get(i11,UUID::RFC4122)<< std::endl;;
	std::cout << UUID::get(s3,UUID::RFC4122)<< std::endl;


}


struct Toto{};
int testStringizer()
{
	using namespace fwTools;


	Toto toto;


	std::string sss("myString");
	int i=10;

	std::pair<int,Toto> couple;//(i,Toto);
	std::map<std::string, float > myMap;
	myMap["un"]=1;
	myMap["deux"]=2;
	myMap["trois"]=3;

	std::cout << "Stringizer::get(typeid(int))" << getString(typeid(int)) << std::endl;

	std::cout << "Stringizer::get(sss)" << getString(sss) << std::endl;
	std::cout << "Stringizer::get(i)" << getString(i)     << std::endl;
	std::cout << "Stringizer::get(toto)" << getString<Toto>(toto)     << std::endl;

	std::cout << "Stringizer::get(toto)" << getString(toto)     << std::endl;
	std::cout << "Stringizer::get(couple)" << getString(couple)     << std::endl;

	std::cout << "Stringizer::get(myMap)" << getString(myMap.begin(),myMap.end())     << std::endl;

	char c=33;
	std::cout << "Stringizer::get(char)" << getString(c)     << std::endl;

	return 0;
}




int main()
{
	std::cout <<  "TestSingleton::getDefault()" << TestSingleton::getDefault() << std::endl;
	std::cout <<  "TestSingleton::getDefault()" << TestSingleton::getDefault() << std::endl;

	testDynamicType();

	testSimpleOK();
	testBinaryCartesianProductOK();
	testBinaryCartesianProductLimitCaseOK();
	testCartesianProductOK();
	testTypeInfoMapping();
	testDispatcherFunctor();

	testClassName();

	testTypeID();
	testUUID();

	testStringizer();

	testRegistrationUnicity();

	return 0;
}
