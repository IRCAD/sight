/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <mfo/Patient.h>
#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>

#include <fwDataIO/reader/TriangularMeshReader.hpp>
#include <fwDataIO/writer/TriangularMeshWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

void test_1(const boost::filesystem::path & trianPath)
{
	SLM_TRACE("test_1");
	::boost::shared_ptr< ::fwData::TriangularMesh> trianMesh(new ::fwData::TriangularMesh());
	::fwDataIO::reader::TriangularMeshReader* reader = new ::fwDataIO::reader::TriangularMeshReader();

	reader->setObject(trianMesh);
	reader->setFile(trianPath);
	reader->read();
	OSLM_TRACE("points().size: " << trianMesh->points().size());
	OSLM_TRACE("points().cells: " << trianMesh->cells().size());
	delete reader;
}

//------------------------------------------------------------------------------

void test_2( const boost::filesystem::path & trianPath1,
			 const boost::filesystem::path & trianPath2 )
{
	SLM_TRACE("test_2");
	::boost::shared_ptr< ::fwData::TriangularMesh> trianMesh1(new ::fwData::TriangularMesh());
	::boost::shared_ptr< ::fwData::TriangularMesh> trianMesh2(new ::fwData::TriangularMesh());

	::fwDataIO::reader::TriangularMeshReader* reader = new ::fwDataIO::reader::TriangularMeshReader();
	::fwDataIO::writer::TriangularMeshWriter* writer = new ::fwDataIO::writer::TriangularMeshWriter();
	reader->setObject(trianMesh1);
	writer->setObject(trianMesh2);

	reader->setFile(trianPath1);
	reader->read();
	writer->setFile(trianPath2);

	trianMesh2->points() = trianMesh1->points();
	trianMesh2->cells() = trianMesh1->cells();
	writer->write();
	delete reader;
	delete writer;
}

//------------------------------------------------------------------------------
int main(int argc, char **argv)
{

    if (argc <= 2)
	{
		SLM_INFO("Syntax helper =>")
		SLM_INFO("./testdataIOTriangularMesh -t1 triangularMeshFile.trian : load a trian.");
		SLM_INFO("./testdataIOTriangularMesh -t2 triangularMeshFile1.trian triangularMeshFile2.trian : load a trian and save it in another file.");
		return 0;
	}

    std::string options = argv[1];

    if ( options == "-t1" )
    {
    	 boost::filesystem::path trianPath (argv[2]);
    	 test_1(trianPath);
    }

    if ( options == "-t2" )
    {
    	 boost::filesystem::path trianPath1 (argv[2]);
    	 boost::filesystem::path trianPath2 (argv[3]);
    	 test_2(trianPath1, trianPath2);
    }
}
