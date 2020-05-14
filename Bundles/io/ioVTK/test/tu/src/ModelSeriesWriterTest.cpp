/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ModelSeriesWriterTest.hpp"

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/generator/SeriesDB.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwTools/System.hpp>

#include <boost/functional/hash.hpp>

#include <filesystem>
#include <set>
#include <string>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioVTK::ut::ModelSeriesWriterTest );

namespace ioVTK
{
namespace ut
{

namespace fs = std::filesystem;
typedef std::vector< std::string >  FileContainerType;

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void runModelSeriesSrv(
    const std::string& impl,
    const SPTR(::fwRuntime::EConfigurationElement)& cfg,
    const SPTR(::fwData::Object)& obj)
{
    ::fwServices::IService::sptr srv = ::fwServices::add(impl);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + impl, srv);

    if (srv->isA("::fwIO::IReader"))
    {
        srv->registerInOut(obj, "data");
    }
    else
    {
        srv->registerInput(obj, "data");
    }

    CPPUNIT_ASSERT_NO_THROW( srv->setConfiguration( cfg ) );
    CPPUNIT_ASSERT_NO_THROW( srv->configure() );
    CPPUNIT_ASSERT_NO_THROW( srv->start().wait() );
    CPPUNIT_ASSERT_NO_THROW( srv->update().wait() );
    CPPUNIT_ASSERT_NO_THROW( srv->stop().wait() );
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr getIOCfgFromFolder(const fs::path& file)
{
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr cfg    = ::fwRuntime::EConfigurationElement::New("folder");
    cfg->setValue(file.string());
    srvCfg->addConfigurationElement(cfg);

    return srvCfg;
}

//------------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr getIOCfgFromFiles(const FileContainerType& files)
{
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");

    for(std::string file :  files)
    {
        ::fwRuntime::EConfigurationElement::sptr cfg = ::fwRuntime::EConfigurationElement::New("file");
        cfg->setValue(file);
        srvCfg->addConfigurationElement(cfg);
    }

    return srvCfg;
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteMeshes()
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwTest::generator::SeriesDB::createModelSeries(5);

    const std::vector< std::string > allExtensions = {"vtk", "vtp", "obj", "ply", "stl"};

    const fs::path dir = ::fwTools::System::getTemporaryFolder() / "modelSeries";

    if( fs::exists(dir) )
    {
        CPPUNIT_ASSERT_MESSAGE(std::string("Directory ") + dir.string() + " must be empty",
                               fs::is_empty(dir));
    }
    else
    {
        fs::create_directories(dir);
    }

    for(auto ext : allExtensions)
    {
        auto cfg    = getIOCfgFromFolder(dir);
        auto extCfg = ::fwRuntime::EConfigurationElement::New("extension");
        extCfg->setValue(ext);
        cfg->addConfigurationElement(extCfg);

        runModelSeriesSrv(
            "::ioVTK::SModelSeriesWriter",
            cfg,
            modelSeries);

        FileContainerType files;
        for(fs::directory_iterator it(dir); it != fs::directory_iterator(); ++it)
        {
            if(it->path().extension() == "." + ext)
            {
                files.push_back(it->path().string());
            }
        }

        CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB().size(), files.size());

        ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

        runModelSeriesSrv(
            "::ioVTK::SSeriesDBReader",
            getIOCfgFromFiles(files),
            seriesDB);

        const ::fwMedData::SeriesDB::ContainerType& series = seriesDB->getContainer();
        CPPUNIT_ASSERT_EQUAL((size_t)1, series.size());

        ::fwMedData::ModelSeries::sptr readSeries = ::fwMedData::ModelSeries::dynamicCast(series[0]);
        CPPUNIT_ASSERT_MESSAGE("A ModelSeries was expected", readSeries);

        typedef ::fwMedData::ModelSeries::ReconstructionVectorType RecVecType;
        const RecVecType& readRecs = readSeries->getReconstructionDB();
        CPPUNIT_ASSERT_EQUAL(files.size(), readRecs.size());

        // Skip comparing mesh with obj/ply/stl format since internal structures aren't the same.
        if(ext != "obj" && ext != "ply" && ext != "stl")
        {
            const RecVecType& refRecs         = modelSeries->getReconstructionDB();
            RecVecType::const_iterator itRef  = refRecs.begin();
            RecVecType::const_iterator itRead = readRecs.begin();

            for(; itRef != refRecs.end(); ++itRef, ++itRead)
            {
                ::fwData::Mesh::csptr refMesh  = (*itRef)->getMesh();
                ::fwData::Mesh::csptr readMesh = (*itRead)->getMesh();

                CPPUNIT_ASSERT_EQUAL(refMesh->getNumberOfPoints(), readMesh->getNumberOfPoints());
                CPPUNIT_ASSERT_EQUAL(refMesh->getNumberOfCells(), readMesh->getNumberOfCells());
                CPPUNIT_ASSERT_EQUAL(refMesh->getCellDataSize(), readMesh->getCellDataSize());

                auto refPointsItr       = refMesh->begin< ::fwData::iterator::ConstPointIterator >();
                auto readPointsItr      = readMesh->begin< ::fwData::iterator::ConstPointIterator >();
                const auto refPointsEnd = refMesh->end< ::fwData::iterator::ConstPointIterator >();

                for(; refPointsItr != refPointsEnd; ++refPointsItr, ++readPointsItr)
                {
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->point->x, readPointsItr->point->x, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->point->y, readPointsItr->point->y, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->point->z, readPointsItr->point->z, 0.00001);

                    CPPUNIT_ASSERT_EQUAL(refPointsItr->rgba->r, readPointsItr->rgba->r);
                    CPPUNIT_ASSERT_EQUAL(refPointsItr->rgba->g, readPointsItr->rgba->g);
                    CPPUNIT_ASSERT_EQUAL(refPointsItr->rgba->b, readPointsItr->rgba->b);
                    CPPUNIT_ASSERT_EQUAL(refPointsItr->rgba->a, readPointsItr->rgba->a);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->normal->nx, readPointsItr->normal->nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->normal->ny, readPointsItr->normal->ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refPointsItr->normal->nz, readPointsItr->normal->nz, 0.00001);
                }

                auto refCellsItr       = refMesh->begin< ::fwData::iterator::ConstCellIterator >();
                auto readCellsItr      = readMesh->begin< ::fwData::iterator::ConstCellIterator >();
                const auto refCellsEnd = refMesh->end< ::fwData::iterator::ConstCellIterator >();

                for(; refCellsItr != refCellsEnd; ++refCellsItr, ++readCellsItr)
                {
                    CPPUNIT_ASSERT_EQUAL(*refCellsItr->type, *readCellsItr->type);
                    CPPUNIT_ASSERT_EQUAL(*refCellsItr->offset, *readCellsItr->offset);
                    CPPUNIT_ASSERT_EQUAL(refCellsItr->nbPoints, readCellsItr->nbPoints);

                    for (size_t i = 0; i < refCellsItr->nbPoints; ++i)
                    {
                        CPPUNIT_ASSERT_EQUAL(refCellsItr->pointIdx[i], readCellsItr->pointIdx[i]);
                    }

                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refCellsItr->normal->nx, readCellsItr->normal->nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refCellsItr->normal->ny, readCellsItr->normal->ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(refCellsItr->normal->nz, readCellsItr->normal->nz, 0.00001);

                    CPPUNIT_ASSERT_EQUAL(refCellsItr->rgba->r, readCellsItr->rgba->r);
                    CPPUNIT_ASSERT_EQUAL(refCellsItr->rgba->g, readCellsItr->rgba->g);
                    CPPUNIT_ASSERT_EQUAL(refCellsItr->rgba->b, readCellsItr->rgba->b);
                    CPPUNIT_ASSERT_EQUAL(refCellsItr->rgba->a, readCellsItr->rgba->a);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteReconstructions()
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwTest::generator::SeriesDB::createModelSeries(5);

    const fs::path dir = ::fwTools::System::getTemporaryFolder() / "modelSeriesObj";

    if( fs::exists(dir) )
    {
        CPPUNIT_ASSERT_MESSAGE(std::string("Directory ") + dir.string() + " must be empty",
                               fs::is_empty(dir));
    }
    else
    {
        fs::create_directories(dir);
    }

    runModelSeriesSrv(
        "::ioVTK::SModelSeriesObjWriter",
        getIOCfgFromFolder(dir),
        modelSeries);

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    FileContainerType files;
    for(fs::directory_iterator it(dir); it != fs::directory_iterator(); ++it)
    {
        files.push_back(it->path().string());
    }

    // Writer generates a .mtl file for each .obj file
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB().size() * 2, files.size());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioVTK
