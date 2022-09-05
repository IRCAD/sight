/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/SeriesDB.hpp>

#include <service/base.hpp>

#include <utestData/generator/SeriesDB.hpp>

#include <boost/functional/hash.hpp>

#include <filesystem>
#include <set>
#include <string>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::ModelSeriesWriterTest);

namespace sight::module::io::vtk::ut
{

namespace fs            = std::filesystem;
using FileContainerType = std::vector<std::string>;

namespace point = sight::data::iterator::point;
namespace cell  = sight::data::iterator::cell;

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
    const SPTR(core::runtime::EConfigurationElement)& cfg,
    const SPTR(data::Object)& obj
)
{
    service::IService::sptr srv = service::add(impl);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + impl, srv);

    if(srv->isA("sight::io::base::service::IReader"))
    {
        srv->setInOut(obj, "data");
    }
    else
    {
        srv->setInput(obj, "data");
    }

    CPPUNIT_ASSERT_NO_THROW(srv->setConfiguration(cfg));
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::remove(srv);
}

//------------------------------------------------------------------------------

core::runtime::EConfigurationElement::sptr getIOCfgFromFolder(const fs::path& file)
{
    core::runtime::EConfigurationElement::sptr srvCfg = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr cfg    = core::runtime::EConfigurationElement::New("folder");
    cfg->setValue(file.string());
    srvCfg->addConfigurationElement(cfg);

    return srvCfg;
}

//------------------------------------------------------------------------------

core::runtime::EConfigurationElement::sptr getIOCfgFromFiles(const FileContainerType& files)
{
    core::runtime::EConfigurationElement::sptr srvCfg = core::runtime::EConfigurationElement::New("service");

    for(const std::string& file : files)
    {
        core::runtime::EConfigurationElement::sptr cfg = core::runtime::EConfigurationElement::New("file");
        cfg->setValue(file);
        srvCfg->addConfigurationElement(cfg);
    }

    return srvCfg;
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteMeshes()
{
    data::ModelSeries::sptr modelSeries = utestData::generator::SeriesDB::createModelSeries(5);

    const std::vector<std::string> allExtensions = {"vtk", "vtp", "obj", "ply", "stl"};

    const fs::path dir = core::tools::System::getTemporaryFolder() / "modelSeries";

    if(fs::exists(dir))
    {
        CPPUNIT_ASSERT_MESSAGE(
            std::string("Directory ") + dir.string() + " must be empty",
            fs::is_empty(dir)
        );
    }
    else
    {
        fs::create_directories(dir);
    }

    for(const auto& ext : allExtensions)
    {
        // Create subfolers per extensions ("/vtk", "/vtp", ...)
        if(fs::exists(dir / ext))
        {
            CPPUNIT_ASSERT_MESSAGE(
                std::string("Directory ") + dir.string() + "/" + ext + " must be empty",
                fs::is_empty(dir / ext)
            );
        }
        else
        {
            fs::create_directories(dir / ext);
        }

        auto cfg    = getIOCfgFromFolder(dir / ext);
        auto extCfg = core::runtime::EConfigurationElement::New("extension");
        extCfg->setValue(ext);
        cfg->addConfigurationElement(extCfg);

        runModelSeriesSrv(
            "sight::module::io::vtk::SModelSeriesWriter",
            cfg,
            modelSeries
        );

        FileContainerType files;
        for(fs::directory_iterator it(dir / ext) ; it != fs::directory_iterator() ; ++it)
        {
            if(it->path().extension() == "." + ext)
            {
                files.push_back(it->path().string());
            }
        }

        // Ensure reading order (modelSeries generator will prefix each file with a number).
        std::sort(files.begin(), files.end());

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Number of saved files",
            modelSeries->getReconstructionDB().size(),
            files.size()
        );

        data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

        runModelSeriesSrv(
            "sight::module::io::vtk::SSeriesDBReader",
            getIOCfgFromFiles(files),
            seriesDB
        );

        const data::SeriesDB::ContainerType& series = seriesDB->getContainer();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("SeriesDB Size", (std::size_t) 1, series.size());

        data::ModelSeries::sptr readSeries = data::ModelSeries::dynamicCast(series[0]);
        CPPUNIT_ASSERT_MESSAGE("A ModelSeries was expected", readSeries);

        using RecVecType = data::ModelSeries::ReconstructionVectorType;
        const RecVecType& readRecs = readSeries->getReconstructionDB();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of reconstructions", files.size(), readRecs.size());

        const RecVecType& refRecs = modelSeries->getReconstructionDB();
        auto itRef                = refRecs.begin();
        auto itRead               = readRecs.begin();

        for( ; itRef != refRecs.end() ; ++itRef, ++itRead)
        {
            data::Mesh::csptr refMesh  = (*itRef)->getMesh();
            data::Mesh::csptr readMesh = (*itRead)->getMesh();

            const auto reflock      = refMesh->dump_lock();
            const auto readMeshLock = readMesh->dump_lock();

            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Number of Points.",
                refMesh->numPoints(),
                readMesh->numPoints()
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Number of Cells.",
                refMesh->numCells(),
                readMesh->numCells()
            );

            // Don't test internal structures for obj, ply and stl, since some of them are missing.
            if(ext != "obj" && ext != "ply" && ext != "stl")
            {
                const auto refPoints  = refMesh->czip_range<point::xyz, point::nxyz, point::rgba>();
                const auto readPoints = readMesh->czip_range<point::xyz, point::nxyz, point::rgba>();

                for(const auto& [ref, read] : boost::combine(refPoints, readPoints))
                {
                    const auto& [pt1, n1, c1] = ref;
                    const auto& [pt2, n2, c2] = read;

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.x ", pt1.x, pt2.x, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.y", pt1.y, pt2.y, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.z", pt1.z, pt2.z, 0.00001);

                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color R", c1.r, c2.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color G", c1.g, c2.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color B", c1.b, c2.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color A", c1.a, c2.a);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal x", n1.nx, n2.nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal y", n1.ny, n2.ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal z", n1.nz, n2.nz, 0.00001);
                }

                const auto refCells  = refMesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();
                const auto readCells = readMesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();

                for(const auto& [ref, read] : boost::combine(refCells, readCells))
                {
                    const auto& [tri1, n1, c1] = ref;
                    const auto& [tri2, n2, c2] = read;

                    for(std::size_t i = 0 ; i < 3 ; ++i)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell point index", tri1.pt[i], tri2.pt[i]);
                    }

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal x", n1.nx, n2.nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal y", n1.ny, n2.ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal z", n1.nz, n2.nz, 0.00001);

                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color R", c1.r, c2.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color G", c1.g, c2.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color B", c1.b, c2.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color A", c1.a, c2.a);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteReconstructions()
{
    data::ModelSeries::sptr modelSeries = utestData::generator::SeriesDB::createModelSeries(5);

    const fs::path dir = core::tools::System::getTemporaryFolder() / "modelSeriesObj";

    if(fs::exists(dir))
    {
        CPPUNIT_ASSERT_MESSAGE(
            std::string("Directory ") + dir.string() + " must be empty",
            fs::is_empty(dir)
        );
    }
    else
    {
        fs::create_directories(dir);
    }

    runModelSeriesSrv(
        "sight::module::io::vtk::SModelSeriesObjWriter",
        getIOCfgFromFolder(dir),
        modelSeries
    );

    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    FileContainerType files;
    for(fs::directory_iterator it(dir) ; it != fs::directory_iterator() ; ++it)
    {
        files.push_back(it->path().string());
    }

    // Writer generates a .mtl file for each .obj file
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB().size() * 2, files.size());
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk::ut
