/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "get_mesh_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::get_mesh_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void get_mesh_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void get_mesh_test::tearDown()
{
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_mesh_by_index()
{
    // Create new Models Series
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);

    // Create Mesh Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2);
    rec3->set_mesh(mesh3);
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1);
    recs.push_back(rec2);
    recs.push_back(rec3);

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(3, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, m_series->get_reconstruction_db()[2]);

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"2\" uid=\"outputMesh1\"/>"
       "<key index=\"0\" uid=\"outputMesh2\"/>"
       "<key index=\"2\" uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    get_mesh_serv->update().wait();

    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 0).lock()->get_id(), mesh3->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 1).lock()->get_id(), mesh1->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 2).lock()->get_id(), mesh3->get_id());
    get_mesh_serv->stop().wait();

    sight::service::remove(get_mesh_serv);
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_mesh_with_index_out_of_bound()
{
    // Create new Models Series
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);

    // Create Mesh Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2);
    rec3->set_mesh(mesh3);
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1);
    recs.push_back(rec2);
    recs.push_back(rec3);

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(3, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, m_series->get_reconstruction_db()[2]);

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"0\" uid=\"outputMesh1\"/>"
       "<key index=\"1\" uid=\"outputMesh2\"/>"
       "<key index=\"6\" uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    CPPUNIT_ASSERT_THROW(get_mesh_serv->update().get(), core::exception);
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_mesh_by_type()
{
    // Create Models Series Object
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);
    sight::data::reconstruction::sptr rec4 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec4);
    sight::data::reconstruction::sptr rec5 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);
    sight::data::mesh::sptr mesh4 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh4);
    sight::data::mesh::sptr mesh5 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh5);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2); //skin, liver , kidney,
    rec3->set_mesh(mesh3);
    rec4->set_mesh(mesh4);
    rec5->set_mesh(mesh5);
    rec1->set_structure_type("skin");
    rec2->set_structure_type("liver");
    rec3->set_structure_type("kidney");
    rec4->set_structure_type("kidney");
    rec5->set_structure_type("kidney");
    rec1->set_organ_name("skin-surface");
    rec2->set_organ_name("liver-surface");
    rec3->set_organ_name("kidney-volume-left");
    rec4->set_organ_name("kidney-volume-right");
    rec5->set_organ_name("kidney-surface");
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);

    service::config_t config;
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

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    get_mesh_serv->update().wait();
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 0).lock()->get_id(), mesh2->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 1).lock()->get_id(), mesh1->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 2).lock()->get_id(), mesh3->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 3).lock()->get_id(), mesh4->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 4).lock()->get_id(), mesh3->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 5).lock()->get_id(), mesh5->get_id());
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_mesh_by_type_not_found()
{
    // Create Models Series Object
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);
    sight::data::reconstruction::sptr rec4 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec4);
    sight::data::reconstruction::sptr rec5 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);
    sight::data::mesh::sptr mesh4 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh4);
    sight::data::mesh::sptr mesh5 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh5);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2);
    rec3->set_mesh(mesh3);
    rec4->set_mesh(mesh4);
    rec5->set_mesh(mesh5);
    rec1->set_structure_type("skin");
    rec2->set_structure_type("liver");
    rec3->set_structure_type("kidney");
    rec4->set_structure_type("kidney");
    rec5->set_structure_type("kidney");
    rec1->set_organ_name("skin-surface");
    rec2->set_organ_name("liver-surface");
    rec3->set_organ_name("kidney-volume-left");
    rec4->set_organ_name("kidney-volume-right");
    rec5->set_organ_name("kidney-surface");
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);

    service::config_t config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key type=\"liver\" uid=\"outputMesh1\"/>"
       "<key type=\"skin\" matching=\"skin-surface\" uid=\"outputMesh2\"/>"
       "<key type=\"gallbladder\"  uid=\"outputMesh3\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    CPPUNIT_ASSERT_THROW(get_mesh_serv->update().get(), sight::data::exception);
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_mesh_by_name_not_found()
{
    // Create Models Series Object
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);
    sight::data::reconstruction::sptr rec4 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec4);
    sight::data::reconstruction::sptr rec5 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);
    sight::data::mesh::sptr mesh4 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh4);
    sight::data::mesh::sptr mesh5 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh5);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2);
    rec3->set_mesh(mesh3);
    rec4->set_mesh(mesh4);
    rec5->set_mesh(mesh5);
    rec1->set_structure_type("skin");
    rec2->set_structure_type("liver");
    rec3->set_structure_type("kidney");
    rec4->set_structure_type("kidney");
    rec5->set_structure_type("kidney");
    rec1->set_organ_name("skin-surface");
    rec2->set_organ_name("liver-surface");
    rec3->set_organ_name("kidney-volume-left");
    rec4->set_organ_name("kidney-volume-right");
    rec5->set_organ_name("kidney-surface");
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);

    service::config_t config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key type=\"liver\" uid=\"outputMesh1\"/>"
       "<key type=\"kidney\" matching=\"azerty\" uid=\"outputMesh2\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    CPPUNIT_ASSERT_THROW(get_mesh_serv->update().get(), sight::data::exception);
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

//------------------------------------------------------------------------------

void get_mesh_test::extracts_with_mesh_type_and_index()
{
    // Create Models Series Object
    auto m_series = std::make_shared<sight::data::model_series>();
    CPPUNIT_ASSERT(m_series);
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    // Create reconstruction object
    sight::data::reconstruction::sptr rec1 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec2);
    sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec3);
    sight::data::reconstruction::sptr rec4 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec4);
    sight::data::reconstruction::sptr rec5 = std::make_shared<sight::data::reconstruction>();
    CPPUNIT_ASSERT(rec5);

    // Create Meshes Object
    sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh3);
    sight::data::mesh::sptr mesh4 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh4);
    sight::data::mesh::sptr mesh5 = std::make_shared<sight::data::mesh>();
    CPPUNIT_ASSERT(mesh5);

    rec1->set_mesh(mesh1);
    rec2->set_mesh(mesh2);
    rec3->set_mesh(mesh3);
    rec4->set_mesh(mesh4);
    rec5->set_mesh(mesh5);
    rec1->set_structure_type("skin");
    rec2->set_structure_type("liver");
    rec3->set_structure_type("kidney");
    rec4->set_structure_type("kidney");
    rec5->set_structure_type("kidney");
    rec1->set_organ_name("skin-surface");
    rec2->set_organ_name("liver-surface");
    rec3->set_organ_name("kidney-volume-left");
    rec4->set_organ_name("kidney-volume-right");
    rec5->set_organ_name("kidney-surface");
    sight::data::model_series::reconstruction_vector_t recs;
    recs.push_back(rec1); // ( skin , skin-surface )
    recs.push_back(rec2); // ( liver , liver-surface )
    recs.push_back(rec3); // ( kidney , kidney-volume-left )
    recs.push_back(rec4); // ( kidney , kidney-volume-right )
    recs.push_back(rec5); // ( kidney , kidney-surface )

    m_series->set_reconstruction_db(recs);
    CPPUNIT_ASSERT_EQUAL(5, (int) m_series->get_reconstruction_db().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->get_reconstruction_db()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->get_reconstruction_db()[1]);

    service::config_t config;
    std::stringstream config_string;
    config_string

    << "<out group=\"mesh\">"
       "<key index=\"4\" uid=\"outputMesh1\"/>"
       "<key type=\"liver\" uid=\"outputMesh2\"/>"
       "<key index=\"0\" uid=\"outputMesh3\"/>"
       "<key type=\"kidney\" matching=\"(.*)volume(.*)\" uid=\"outputMesh4\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(m_series, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    get_mesh_serv->update().wait();
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 0).lock()->get_id(), mesh5->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 1).lock()->get_id(), mesh2->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 2).lock()->get_id(), mesh1->get_id());
    CPPUNIT_ASSERT_EQUAL(get_mesh_serv->output("mesh", 3).lock()->get_id(), mesh3->get_id());
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

// //------------------------------------------------------------------------------

void get_mesh_test::extracts_with_invalid_models_series()
{
    // Create service
    sight::service::base::sptr get_mesh_serv = sight::service::add("sight::module::data::get_mesh");
    CPPUNIT_ASSERT(get_mesh_serv);
    CPPUNIT_ASSERT(get_mesh_serv->is_a("sight::module::data::get_mesh"));

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"mesh\">"
       "<key index=\"0\" uid=\"outputMesh1\"/>"
       "<key index=\"1\" uid=\"outputMesh2\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    get_mesh_serv->set_config(config);
    get_mesh_serv->set_input(nullptr, "modelSeries");
    get_mesh_serv->configure();
    get_mesh_serv->start().wait();
    CPPUNIT_ASSERT_THROW(get_mesh_serv->update().get(), sight::data::exception);
    get_mesh_serv->stop().wait();
    sight::service::remove(get_mesh_serv);
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
