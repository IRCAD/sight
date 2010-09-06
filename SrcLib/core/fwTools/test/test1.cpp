/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "bind1.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/ClassRegistrar.hpp"
#include "fwTools/ClassFactory.hpp"


int test1()
{


    ClassFactory<Base1,A11,int> c1(1);
    ClassFactory<Base1,A12,int> c2(2);
    ClassFactory<Base1,A13,int> c3(3);


    assert ( c2.baseClassId() == typeid(Base1)   );
    assert ( c2.baseClassId() != typeid(A12)   );
    assert ( c2.keyId() != typeid(float)   );
    assert ( c2.keyId() == typeid(int)   );

     ::boost::shared_ptr< Base1 > a12 = c2.create();
    assert( dynamic_cast<A12 *>( (a12.get()) ) );

    // contexte d'application
    // une factory d'object en fonction d'un nom
    // classFactory< IDataObjectFactory, ImageFactory, std::string> c_x("Image");


    ClassFactory<Base2,A21, std::type_info  > cc1( typeid(Object1) );
    ClassFactory<Base2,A22, std::type_info  > cc2( typeid(Object2) );
    ClassFactory<Base2,A22, std::type_info  > cc3( typeid(Object3) );

    // contexte d'application des methode des serialisation piur différents type
    //ClassFactory< Serialization , FieldSerialization , std::type_info  > cc_x( typeid(Field) );

    return 0;

}


int test2()
{
    // a noter que la clef peut-être également la classe de BASE !!!!
    // ce concept marche !!!!
    // une classe à moitié créée peut se voir ajouter des fonctionalités en une classe dérivée !!!!
    ClassFactory<Base1,A11,Base1> c1(Base1());
    //ClassFactory<Base1,A12,Base1> c1(Base1(otherparam));


    // il est possible de creer plusieurs clef pour la meme fonctionalité
    ClassFactory<Base1,A11,int> cc1(1);
    ClassFactory<Base1,A11,std::string> ccc1("un");

    // il est possible d'ajouter la même fonctionalité aux mêmes objets !
    ClassFactory<Base1,A11,std::type_info> d1( typeid(Object1) );
    ClassFactory<Base1,A11,std::type_info> d2( typeid(Object2) );
    ClassFactory<Base1,A11,std::type_info> d3( typeid(Object3) );

    return 0;

}


int test3()
{
     ::boost::shared_ptr< ClassFactory<Base1,A11,int> > c1( new ClassFactory<Base1,A11,int>(1) );
     ::boost::shared_ptr< ClassFactory<Base1,A12,int> > c2( new ClassFactory<Base1,A12,int>(2) );

    ClassFactoryRegistry::addFactory( c1 );
    ClassFactoryRegistry::addFactory( c2 );


     ::boost::shared_ptr< Base1 > b;
    int key = 1;
    b = ClassFactoryRegistry::create< Base1 >(key);
    assert ( dynamic_cast< A11 *>(b.get()) );
    assert ( dynamic_cast< A12 *>(b.get()) == 0  );

    key=12345324;
    b = ClassFactoryRegistry::create< Base1 >(key);
    assert( b.get()==NULL );


    std::string skey("invalidKey");
    b = ClassFactoryRegistry::create< Base1 >(skey); // compil but never return a Base1
    assert( b.get()==NULL );

    return 0;



}

// test if this code compile --> OK
static const std::type_info &toto=typeid(std::string);
static ClassRegistrar< Base2, A21 , std::type_info > registrar0( toto );

static const int &titi=1032323;
static ClassRegistrar< Base1, A11 , int > registrari( titi );



// register Object;
REGISTER_BINDING( Base1 , A11, int , 10 ); // can be used if standalone in a cpp
REGISTER_BINDING( Base1 , A12, int , 20 ); // other should use dummy id
REGISTER_BINDING( Base1 , A13, int , 30 );


REGISTER_BINDING( Base2 , A21, std::type_info , typeid( Object1 )  );
REGISTER_BINDING( Base2 , A22, std::type_info , typeid( Object2 )  );

int test4()
{
     ::boost::shared_ptr< Base1 > b;
    int key = 10;
    b = ClassFactoryRegistry::create< Base1 >(key);
    assert ( dynamic_cast< A11 *>(b.get()) );
    assert ( dynamic_cast< A12 *>(b.get()) == 0  );

    return 0;
}

int test5()
{
     ::boost::shared_ptr< Base2 > b;

    b = ClassFactoryRegistry::create< Base2 >( typeid(Object1) );
    assert ( dynamic_cast< A21 *>(b.get()) );
    assert ( dynamic_cast< A22 *>(b.get()) == 0  );

    return 0;
}




int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
