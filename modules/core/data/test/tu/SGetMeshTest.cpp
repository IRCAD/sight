/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SGetMeshTest.hpp"

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <service/base.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SGetMeshTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SGetMeshTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::loadModule(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetMeshTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsMeshByIndex()
{
    // Create new Models Series
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);

    // Create Mesh Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);
    rec3->setMesh(mesh3);
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    recs.push_back(rec2);
    recs.push_back(rec3);

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(3, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, m_series->getReconstructionDB()[2]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"2\" uid=\"outputMesh1\"/>"
       "<key index=\"0\" uid=\"outputMesh2\"/>"
       "<key index=\"2\" uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    getMeshServ->update().wait();

    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 0).lock()->getID(), mesh3->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 1).lock()->getID(), mesh1->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 2).lock()->getID(), mesh3->getID());
    getMeshServ->stop().wait();

    sight::service::remove(getMeshServ);
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsMeshWithIndexOutOfBound()
{
    // Create new Models Series
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);

    // Create Mesh Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);
    rec3->setMesh(mesh3);
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    recs.push_back(rec2);
    recs.push_back(rec3);

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(3, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, m_series->getReconstructionDB()[2]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"0\" uid=\"outputMesh1\"/>"
       "<key index=\"1\" uid=\"outputMesh2\"/>"
       "<key index=\"6\" uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    CPPUNIT_ASSERT_THROW(getMeshServ->update().get(), core::Exception);
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsMeshByType()
{
    // Create Models Series Object
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);
    sight::data::Reconstruction::sptr rec4 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec4);
    sight::data::Reconstruction::sptr rec5 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);
    sight::data::Mesh::sptr mesh4 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh4);
    sight::data::Mesh::sptr mesh5 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh5);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2); //skin, liver , kidney,
    rec3->setMesh(mesh3);
    rec4->setMesh(mesh4);
    rec5->setMesh(mesh5);
    rec1->setStructureType("skin");
    rec2->setStructureType("liver");
    rec3->setStructureType("kidney");
    rec4->setStructureType("kidney");
    rec5->setStructureType("kidney");
    rec1->setOrganName("skin-surface");
    rec2->setOrganName("liver-surface");
    rec3->setOrganName("kidney-volume-left");
    rec4->setOrganName("kidney-volume-right");
    rec5->setOrganName("kidney-surface");
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key type=\"liver\" uid=\"outputMesh1\"/>"
       "<key type=\"skin\" matching=\"skin-surface\" uid=\"outputMesh2\"/>"
       "<key type=\"kidney\" uid=\"outputMesh3\"/>"
       "<key type=\"kidney\" matching=\"kidney-volume-right\" uid=\"outputMesh4\"/>"
       "<key type=\"kidney\" matching=\"(.*)volume(.*)\" uid=\"outputMesh4\"/>"
       "<key type=\"kidney\" matching=\"(.*)surface(.*)\" uid=\"outputMesh5\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    getMeshServ->update().wait();
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 0).lock()->getID(), mesh2->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 1).lock()->getID(), mesh1->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 2).lock()->getID(), mesh3->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 3).lock()->getID(), mesh4->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 4).lock()->getID(), mesh3->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 5).lock()->getID(), mesh5->getID());
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsMeshByTypeNotFound()
{
    // Create Models Series Object
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);
    sight::data::Reconstruction::sptr rec4 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec4);
    sight::data::Reconstruction::sptr rec5 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);
    sight::data::Mesh::sptr mesh4 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh4);
    sight::data::Mesh::sptr mesh5 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh5);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);
    rec3->setMesh(mesh3);
    rec4->setMesh(mesh4);
    rec5->setMesh(mesh5);
    rec1->setStructureType("skin");
    rec2->setStructureType("liver");
    rec3->setStructureType("kidney");
    rec4->setStructureType("kidney");
    rec5->setStructureType("kidney");
    rec1->setOrganName("skin-surface");
    rec2->setOrganName("liver-surface");
    rec3->setOrganName("kidney-volume-left");
    rec4->setOrganName("kidney-volume-right");
    rec5->setOrganName("kidney-surface");
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key type=\"liver\" uid=\"outputMesh1\"/>"
       "<key type=\"skin\" matching=\"skin-surface\" uid=\"outputMesh2\"/>"
       "<key type=\"gallbladder\"  uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    CPPUNIT_ASSERT_THROW(getMeshServ->update().get(), sight::data::Exception);
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsMeshByNameNotFound()
{
    // Create Models Series Object
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);
    sight::data::Reconstruction::sptr rec4 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec4);
    sight::data::Reconstruction::sptr rec5 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);
    sight::data::Mesh::sptr mesh4 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh4);
    sight::data::Mesh::sptr mesh5 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh5);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);
    rec3->setMesh(mesh3);
    rec4->setMesh(mesh4);
    rec5->setMesh(mesh5);
    rec1->setStructureType("skin");
    rec2->setStructureType("liver");
    rec3->setStructureType("kidney");
    rec4->setStructureType("kidney");
    rec5->setStructureType("kidney");
    rec1->setOrganName("skin-surface");
    rec2->setOrganName("liver-surface");
    rec3->setOrganName("kidney-volume-left");
    rec4->setOrganName("kidney-volume-right");
    rec5->setOrganName("kidney-surface");
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key type=\"liver\" uid=\"outputMesh1\"/>"
       "<key type=\"kidney\" matching=\"azerty\" uid=\"outputMesh2\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    CPPUNIT_ASSERT_THROW(getMeshServ->update().get(), sight::data::Exception);
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

//------------------------------------------------------------------------------

void SGetMeshTest::extractsWithMeshTypeAndIndex()
{
    // Create Models Series Object
    auto m_series = sight::data::ModelSeries::New();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);

    // Create reconstruction object
    sight::data::Reconstruction::sptr rec1 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    sight::data::Reconstruction::sptr rec2 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);
    sight::data::Reconstruction::sptr rec3 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec3);
    sight::data::Reconstruction::sptr rec4 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec4);
    sight::data::Reconstruction::sptr rec5 = sight::data::Reconstruction::New();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::Mesh::sptr mesh1 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    sight::data::Mesh::sptr mesh2 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    sight::data::Mesh::sptr mesh3 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh3);
    sight::data::Mesh::sptr mesh4 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh4);
    sight::data::Mesh::sptr mesh5 = sight::data::Mesh::New();
    CPPUNIT_ASSERT(mesh5);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);
    rec3->setMesh(mesh3);
    rec4->setMesh(mesh4);
    rec5->setMesh(mesh5);
    rec1->setStructureType("skin");
    rec2->setStructureType("liver");
    rec3->setStructureType("kidney");
    rec4->setStructureType("kidney");
    rec5->setStructureType("kidney");
    rec1->setOrganName("skin-surface");
    rec2->setOrganName("liver-surface");
    rec3->setOrganName("kidney-volume-left");
    rec4->setOrganName("kidney-volume-right");
    rec5->setOrganName("kidney-surface");
    sight::data::ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key index=\"4\" uid=\"outputMesh1\"/>"
       "<key type=\"liver\" uid=\"outputMesh2\"/>"
       "<key index=\"0\" uid=\"outputMesh3\"/>"
       "<key type=\"kidney\" matching=\"(.*)volume(.*)\" uid=\"outputMesh4\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(m_series, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    getMeshServ->update().wait();
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 0).lock()->getID(), mesh5->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 1).lock()->getID(), mesh2->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 2).lock()->getID(), mesh1->getID());
    CPPUNIT_ASSERT_EQUAL(getMeshServ->getOutput<sight::data::Object>("mesh", 3).lock()->getID(), mesh3->getID());
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

// //------------------------------------------------------------------------------

void SGetMeshTest::extractsWithInvalidModelsSeries()
{
    // Create service
    sight::service::IService::sptr getMeshServ = sight::service::add("sight::module::data::SGetMesh");
    CPPUNIT_ASSERT(getMeshServ);
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"0\" uid=\"outputMesh1\"/>"
       "<key index=\"1\" uid=\"outputMesh2\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getMeshServ->setConfiguration(config);
    getMeshServ->setInput(nullptr, "modelSeries");
    getMeshServ->configure();
    getMeshServ->start().wait();
    CPPUNIT_ASSERT_THROW(getMeshServ->update().get(), sight::data::Exception);
    getMeshServ->stop().wait();
    sight::service::remove(getMeshServ);
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
