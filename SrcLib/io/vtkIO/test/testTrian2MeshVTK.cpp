/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>
#include <fwDataIO/reader/TriangularMeshReader.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwTools/Factory.hpp>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkIO/vtk.hpp>


int main(int argc, char **argv)
{
        if (argc!=3)
        {
                std::cout << "Transform a Trian to an VTK Mesh\nUsage " << *argv << "input.trian output.vtk";
                return 0;
        }

        try
        {
        char *trianFile=argv[1];
        char *vtkMesh=argv[2];

        namespace fs = boost::filesystem;

        fs::path trianPath(trianFile);
        fs::path meshPath(vtkMesh);


        ::fwData::TriangularMesh::sptr trian= ::fwTools::Factory::New< ::fwData::TriangularMesh>();
        assert(trian);
        ::fwDataIO::reader::TriangularMeshReader trianReader;
        trianReader.setObject(trian);
        trianReader.setFile(trianPath.string());
        trianReader.read();

        assert( trian->points().size() );

        vtkPolyData *mesh = ::vtkIO::toVTKMesh( trian );

        vtkPolyDataWriter* wri = vtkPolyDataWriter::New();
        wri->SetFileTypeToBinary();
        wri->SetInput(0,mesh);
        wri->SetFileName(meshPath.string().c_str());
        wri->Write();
        }
        catch (std::exception &e)
        {
                std::cerr << argv[0] << "  Fail " << e.what();
        }

        return 0;

}
