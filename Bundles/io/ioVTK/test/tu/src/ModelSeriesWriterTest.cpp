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
                CPPUNIT_ASSERT(::fwTest::helper::compare((*itRef)->getMesh(), (*itRead)->getMesh()));
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
