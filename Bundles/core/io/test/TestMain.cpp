/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <iostream>
#include <fstream>
#include "io/serializer/Region.hpp"
#include "io/serializer/Image.hpp"

#include "op/algo/Algorithm.hpp"


#include "fwData/Region.hpp"



#include <fwRuntime/Executable.hpp>
class TestBundle : public ::fwRuntime::Executable
{
    public :
    int m_value;
};



/*
int testRegionObjectComposite()
{
     ::boost::shared_ptr< ::fwData::Region>  maRegion( new ::fwData::Region(3));
    
    maRegion->getOrigin()[0] = 2 ;
    maRegion->getOrigin()[1] = 3 ;
    maRegion->getOrigin()[2] = 5 ;
    
    maRegion->getSize()[0] = 11 ;
    maRegion->getSize()[1] = 13 ;
    maRegion->getSize()[2] = 17 ;
    
//  std::ofstream ofs_xml("/tmp/testRegion.xml");
//  boost::archive::xml_oarchive ao_xml(ofs_xml);
//  ao_xml & boost::serialization::make_nvp("UneRegion",maRegion);
    
    
    ::fwData::Region bigRegion(2);
    
    bigRegion.getOrigin()[0] = 20 ;
    bigRegion.getOrigin()[1] = 30 ;
    
    bigRegion.getSize()[0] = 22 ;
    bigRegion.getSize()[1] = 33 ;
    
    bigRegion.addField( std::string("subregion"),maRegion);
    
     ::boost::shared_ptr< ::fwData::Object>  obj = maRegion;
    
    {
    std::ofstream ofs2_xml("/tmp/2testRegion.xml");
    boost::archive::xml_oarchive ao2_xml(ofs2_xml);
    ao2_xml & boost::serialization::make_nvp("lacomposition",bigRegion);
    //ao2_xml & boost::serialization::make_nvp("lacomposition",obj);
    ofs2_xml.close();
    }
    
    // test for reloading ...
    std::ifstream ifs2_xml("/tmp/2testRegion.xml");
    boost::archive::xml_iarchive ai2_xml(ifs2_xml);
     ::boost::shared_ptr< ::fwData::Region>  objt ( new ::fwData::Region ); // must be instancied;
    ai2_xml & boost::serialization::make_nvp( "lacomposition", *objt );
    
    // check data
     ::boost::shared_ptr< ::fwData::Object>  objs =  objt->getField("subregion");
    ::fwData::Region *regt=dynamic_cast< ::fwData::Region *>(objs.get());
    if ( regt->getSize()[1]==13 )
    {
        std::cout << "load+save OK";
    }
    else
    {
        std::cout << "load+save fail";
    }
    
}


int testImage()
{
    ::fwData::Image image;
    image.getDimension()=3;
    image.getSpacing()= std::vector<float>(3);
    image.getRegion().getOrigin()= std::vector<float>(3);
    image.getRegion().getSize()= std::vector<int32>(3);
    
    
    
    image.getSpacing()[0]=1.2;image.getSpacing()[1]=2.3;image.getSpacing()[2]=3.4;
    image.getRegion().getOrigin()[0]=-0.1;image.getRegion().getOrigin()[1]=-0.2;image.getRegion().getOrigin()[2]=-0.3;
    
    image.getRegion().getSize()[0]=128;image.getRegion().getSize()[1]=256;image.getRegion().getSize()[2]=512;
    
    image.setPixelType("short int");
    image.setBuffer( new short int[128*256*512] );
    
    {
    std::ofstream ofs_xml("/tmp/testImage.xml");
    boost::archive::xml_oarchive ao_xml(ofs_xml);
    ao_xml & boost::serialization::make_nvp("UneImage",image);
    ofs_xml.close();
    }
    
    // test for reload
    std::ifstream ifs_xml("/tmp/testImage.xml");
    boost::archive::xml_iarchive ai_xml(ifs_xml);
    ai_xml & boost::serialization::make_nvp("UneImage",image);
    ifs_xml.close();    
}
*/
int main()
{
     ::boost::shared_ptr<op::algo::Algorithm> algo(new op::algo::Algorithm() );
     ::boost::shared_ptr< ::fwData::Float > v1( new ::fwData::Float );
     ::boost::shared_ptr< ::fwData::Float > v2( new ::fwData::Float );
    v1->value()=12.2;
    v2->value()=23.4;
    
    
    algo->getParameters().insert( std::make_pair("valeur1ID",v1) );
    algo->getParameters().insert( std::make_pair("valeur1ID",v2) );
    
    

}

int main3()
{
    TestBundle tb;
}
