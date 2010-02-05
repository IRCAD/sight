/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/writer/FwXMLObjectWriter.hpp"
#include "fwXML/reader/FwXMLObjectReader.hpp"

//------------------------------------------------------------------------------

::fwData::PointList::sptr buildPointList()
{
    ::fwData::PointList::NewSptr pl;

    // P1
    ::fwData::Point::NewSptr p1;
    p1->getRefCoord()[0] = 1.0;
    p1->getRefCoord()[1] = 1.1;
    p1->getRefCoord()[2] = 1.2;
    pl->getRefPoints().push_back(p1);

    // P2
    ::fwData::Point::NewSptr p2;
    p2->getRefCoord()[0] = 2.0;
    p2->getRefCoord()[1] = 2.1;
    p2->getRefCoord()[2] = 2.2;
    pl->getRefPoints().push_back(p2);
    ::fwData::String::NewSptr str1;
    str1->value() = "toto";
    p2->setFieldSingleElement("TMP_VAL_1", str1);

    // P3
    ::fwData::Point::NewSptr p3;
    p3->getRefCoord()[0] = 3.0;
    p3->getRefCoord()[1] = 3.1;
    p3->getRefCoord()[2] = 3.2;
    pl->getRefPoints().push_back(p3);

    ::fwData::String::NewSptr str0;
    str0->value() = "titi";
    pl->setFieldSingleElement("TMP_VAL_0", str0);

    return pl;
}

//------------------------------------------------------------------------------

void printPointList( ::fwData::PointList::sptr pPointList )
{
    // Print points
    for (   std::vector< ::fwData::Point::sptr >::iterator pointIter = pPointList->getRefPoints().begin();
            pointIter != pPointList->getRefPoints().end();
            ++pointIter )
    {
        double x = (*pointIter)->getRefCoord()[0];
        double y = (*pointIter)->getRefCoord()[1];
        double z = (*pointIter)->getRefCoord()[2];
        OSLM_INFO( "Point => " << x << "  -  " << y << "  -  " << z);
    }
}

//------------------------------------------------------------------------------

int main()
{
    ::fwData::PointList::sptr pl = buildPointList();
    printPointList( pl );


    ::fwXML::writer::FwXMLObjectWriter writer;
    writer.setObject( pl );
    writer.setFile( boost::filesystem::path("/tmp/test.xml") );
    writer.write();


    ::fwXML::reader::FwXMLObjectReader reader;
    reader.setFile( boost::filesystem::path("/tmp/test.xml") );
    reader.read();

    ::fwData::PointList::sptr newPl = ::fwData::PointList::dynamicCast( reader.getObject() );
    printPointList( newPl );

    writer.setObject( newPl );
    writer.setFile( boost::filesystem::path("/tmp/test2.xml") );
    writer.write();
}

