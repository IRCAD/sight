/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "series_set_reader_test.hpp"

#include <core/log/logger.hpp>
#include <core/memory/buffer_manager.hpp>

#include <data/color.hpp>
#include <data/dicom_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/reconstruction.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest/filter.hpp>

#include <utest_data/data.hpp>
#include <utest_data/dicom_reader_test.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/compute/detail/sha1.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cppunit/extensions/HelperMacros.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::series_set_reader_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

std::string get_value(
    const boost::property_tree::ptree& _node,
    const std::string& _name,
    const std::filesystem::path& _file_path
)
{
    std::string value;
    try
    {
        value = _node.get<std::string>(_name);

        // Remove leading and trailing spaces
        std::size_t first = value.find_first_not_of(' ');
        if(first != std::string::npos)
        {
            std::size_t last = value.find_last_not_of(' ');
            value = value.substr(first, last - first + 1);
        }
    }
    catch(boost::property_tree::ptree_bad_path&)
    {
        SIGHT_WARN(_name + " information are missing in '" + _file_path.string() + "'.");
        value = "";
    }

    return value;
}

//------------------------------------------------------------------------------

/**
 * @brief Verify tag values according to JSON files generated from DICOM dump
 * @param filename DICOM folder name
 * @param series_set series_set object containing DICOM series
 */
void verify_tag_values(const std::string& _filename, const data::series_set::sptr& _series_set)
{
    const double delta                     = 0.001;
    const std::filesystem::path dicom_path = utest_data::dir() / "sight/Patient/Dicom/DicomDB";
    const std::filesystem::path meta_path  = dicom_path / "META";

    for(const auto& object : *_series_set)
    {
        auto series = std::dynamic_pointer_cast<data::image_series>(object);

        // Parse META File
        const std::string meta_name           = _filename + "/" + series->get_series_instance_uid() + ".json";
        const std::filesystem::path meta_file = meta_path / meta_name;
        const std::string mf                  = meta_file.string();
        boost::property_tree::ptree root;
        boost::property_tree::json_parser::read_json(mf, root);

        // Series
        CPPUNIT_ASSERT_EQUAL(get_value(root, "SeriesInstanceUID", mf), series->get_series_instance_uid());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "Modality", mf), series->get_modality());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "SeriesDate", mf), series->get_series_date());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "SeriesTime", mf), series->get_series_time());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "SeriesDescription", mf), series->get_series_description());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PerformingPhysicianName", mf), series->get_performing_physician_name());

        // Patient
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PatientID", mf), series->get_patient_id());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PatientName", mf), series->get_patient_name());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PatientBirthDate", mf), series->get_patient_birth_date());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PatientSex", mf), series->get_patient_sex());

        // Study
        CPPUNIT_ASSERT_EQUAL(get_value(root, "StudyInstanceUID", mf), series->get_study_instance_uid());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "StudyDate", mf), series->get_study_date());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "StudyTime", mf), series->get_study_time());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "ReferringPhysicianName", mf), series->get_referring_physician_name());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "StudyDescription", mf), series->get_study_description());
        CPPUNIT_ASSERT_EQUAL(get_value(root, "PatientAge", mf), series->get_patient_age());

        // Equipment
        CPPUNIT_ASSERT_EQUAL(get_value(root, "InstitutionName", mf), series->get_institution_name());

        // PixelSpacing - Not checked as the image could be rotated

        // SliceThickness - This value is recomputed using the slice_thickness_modifier filter.
//        std::string spacingZ = getValue(root, "SliceThickness", mf);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(boost::lexical_cast< double >(spacingZ), image->spacing()[2], 0.0001);

        // Origin - Not checked as the image could be rotated

        // Size - Not checked as the image could be rotated

        // Window Center
        const std::string window_center = get_value(root, "WindowCenter", mf);
        if(!window_center.empty())
        {
            std::vector<std::string> window_center_values;
            boost::split(window_center_values, window_center, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                boost::lexical_cast<double>(window_center_values[0]),
                series->window_center().front(),
                delta
            );
        }
        else
        {
            CPPUNIT_ASSERT(series->window_center().empty());
        }

        // Window Width
        const std::string window_width = get_value(root, "WindowWidth", mf);
        if(!window_width.empty())
        {
            std::vector<std::string> window_width_values;
            boost::split(window_width_values, window_width, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                boost::lexical_cast<double>(window_width_values[0]),
                series->window_width().front(),
                delta
            );
        }
        else
        {
            CPPUNIT_ASSERT(series->window_width().empty());
        }

        // Number of components
        const std::string photometric_interpretation = get_value(root, "PhotometricInterpretation", mf);
        std::size_t nb_components                    = 0;
        if(photometric_interpretation == "MONOCHROME2")
        {
            nb_components = 1;
        }
        else if(photometric_interpretation == "RGB" || photometric_interpretation == "YBR")
        {
            nb_components = 3;
        }
        else if(photometric_interpretation == "ARGB" || photometric_interpretation == "CMYK")
        {
            nb_components = 4;
        }

        CPPUNIT_ASSERT_EQUAL(nb_components, series->num_components());
    }
}

//------------------------------------------------------------------------------

void series_set_reader_test::setUp()
{
    // Set up context before running a test.
    if(utest::filter::ignore_slow_tests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void series_set_reader_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_jms_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_jms_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_ct_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_mr_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ot_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_ot_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_seg_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_seg_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_sf_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_sf_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_sr_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_sr_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_3d_sr_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_3d_sr_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_disabled_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_disabled_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_series_with_dicom_dir_db_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_mr_series_with_dicom_dir();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_multiple_rescale_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_multiple_rescale_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_with_surview_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_ct_with_surview_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_with_temporal_position_series_set_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_mr_with_temporal_position_series();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_series_set_issue01_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::direct);
    series_set_reader_test::read_ct_series_set_issue01();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_jms_series()
{
    auto series_set = std::make_shared<data::series_set>();
    // cspell: ignore Genou
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/JMSGenou";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    data::image_series::sptr series = std::dynamic_pointer_cast<data::image_series>(series_set->front());

    // Check trimmed values
    CPPUNIT_ASSERT(utest_data::dicom_reader_test::check_series_jms_genou_trimmed(series));

    // Read image in lazy mode
    const auto dump_lock = series->dump_lock();
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "01-CT-DICOM_LIVER";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.00001;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->num_dimensions());

    // Check size
    const data::image::size_t size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(129), size[2]);

    // Check spacing
    const data::image::spacing_t spacing = series->spacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.6), spacing[2], delta);

    // Check origin
    const data::image::origin_t origin = series->origin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, series->window_center().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, series->window_width().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::INT16, series->type());

    // Verify tag values according to json file
    verify_tag_values(filename, series_set);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->num_dimensions());

    // Check size
    const data::image::size_t size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[2]);

    // Check spacing
    const data::image::spacing_t spacing = series->spacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[2], delta);

    // Check origin
    const data::image::origin_t origin = series->origin();
    SIGHT_WARN("ORIGIN : " << origin[0] << " " << origin[1] << " " << origin[2]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-112.828), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-180.058), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(97.1478), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1000), series->window_center().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2000), series->window_width().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, series->type());

    // Verify tag values according to json file
    verify_tag_values(filename, series_set);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ot_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "42-OT-BARRE-MONO2-8-colon";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->num_dimensions());

    // Check size
    data::image::size_t size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

    // Check spacing
    data::image::spacing_t spacing = series->spacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[2], delta);

    // Check origin
    data::image::origin_t origin = series->origin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT(series->window_center().empty());

    // Check window width
    CPPUNIT_ASSERT(series->window_width().empty());

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, series->type());

    // Verify tag values according to json file
    verify_tag_values(filename, series_set);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_seg_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "71-CT-DICOM_SEG";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    //Retrieve ImageSeries
    data::model_series::sptr series = std::dynamic_pointer_cast<data::model_series>((*series_set)[1]);
    CPPUNIT_ASSERT(series);

    data::model_series::reconstruction_vector_t reconstruction_db = series->get_reconstruction_db();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), reconstruction_db.size());

    // Check reconstruction
    data::reconstruction::sptr reconstruction = reconstruction_db[0];
    CPPUNIT_ASSERT_EQUAL(std::string("Liver"), reconstruction->get_organ_name());

    // Check mesh
    data::mesh::sptr mesh = reconstruction->get_mesh();
    CPPUNIT_ASSERT_EQUAL(data::mesh::size_t(2498), mesh->num_points());
    CPPUNIT_ASSERT_EQUAL(data::mesh::size_t(5000), mesh->num_cells());

    // Check material
    data::material::sptr material = reconstruction->get_material();
    data::color::sptr color       = material->diffuse();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.8), color->red(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), color->green(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), color->blue(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), color->alpha(), delta);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_sf_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "71-CT-DICOM_SF";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Retrieve landmarks
    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*series);

    // Verify first landmark
    const data::point::sptr& point_a = point_list->get_points()[0];
    const std::string label_a        = point_a->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), label_a);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), point_a->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), point_a->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), point_a->get_coord()[2], delta);

    // Verify second landmark
    const data::point::sptr& point_b = point_list->get_points()[1];
    const std::string label_b        = point_b->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), label_b);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), point_b->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), point_b->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_b->get_coord()[2], delta);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_sr_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "71-CT-DICOM_SR";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    CPPUNIT_ASSERT(series);
    const auto dump_lock = series->dump_lock();

    // Retrieve landmarks
    data::point_list::sptr landmark_point_list = data::helper::medical_image::get_landmarks(*series);

    // Verify first landmark
    const data::point::sptr& point_a = landmark_point_list->get_points()[0];
    const std::string label_a        = point_a->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), label_a);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), point_a->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), point_a->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), point_a->get_coord()[2], delta);

    // Verify second landmark
    const data::point::sptr& point_b = landmark_point_list->get_points()[1];
    const std::string label_b        = point_b->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), label_b);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), point_b->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), point_b->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_b->get_coord()[2], delta);

    // Retrieve distances
    data::vector::sptr distance_vector = data::helper::medical_image::get_distances(*series);

    // Verify first distance
    auto distance_point_list         = std::dynamic_pointer_cast<data::point_list>((*distance_vector)[0]);
    const data::point::sptr& point_c = distance_point_list->get_points()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(38.34), point_c->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(116.67), point_c->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_c->get_coord()[2], delta);
    const data::point::sptr& point_d = distance_point_list->get_points()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(329.41), point_d->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(302.33), point_d->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_d->get_coord()[2], delta);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_3d_sr_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "71-CT-DICOM_3DSR";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Retrieve landmarks
    data::point_list::sptr landmark_point_list = data::helper::medical_image::get_landmarks(*series);
    // Verify first landmark
    const data::point::sptr& point_a = landmark_point_list->get_points()[0];
    const std::string label_a        = point_a->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), label_a);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), point_a->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), point_a->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), point_a->get_coord()[2], delta);

    // Verify second landmark
    const data::point::sptr& point_b = landmark_point_list->get_points()[1];
    const std::string label_b        = point_b->get_label();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), label_b);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), point_b->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), point_b->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_b->get_coord()[2], delta);

    // Retrieve distances
    data::vector::sptr distance_vector = data::helper::medical_image::get_distances(*series);

    // Verify first distance
    auto distance_point_list         = std::dynamic_pointer_cast<data::point_list>((*distance_vector)[0]);
    const data::point::sptr& point_c = distance_point_list->get_points()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), point_c->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(99.30), point_c->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(87.26), point_c->get_coord()[2], delta);
    const data::point::sptr& point_d = distance_point_list->get_points()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(329.41), point_d->get_coord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(302.33), point_d->get_coord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), point_d->get_coord()[2], delta);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_disabled_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<std::string> supported_sop_class_container;
    supported_sop_class_container.emplace_back("1.2.840.10008.5.1.4.1.1.2"); // CT Image Storage

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");
    reader->setsupported_sop_class_container(supported_sop_class_container);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), std::size_t(reader->get_logger()->count(core::log::log::critical)));
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_series_with_dicom_dir()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");
    reader->set_dicomdir_activated(true);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has succeed
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->get_logger()->count(core::log::log::critical)));
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_multiple_rescale_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "83-CT-MultipleRescale";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Retrieve ImageSeries
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    auto series          = std::dynamic_pointer_cast<data::image_series>(series_set->front());
    const auto dump_lock = series->dump_lock();

    // Get internal buffer
    auto* buffer = series->buffer();
    CPPUNIT_ASSERT(buffer);

    // Compute sha1 digest
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(static_cast<char*>(buffer), series->size_in_bytes());
    boost::uuids::detail::sha1::digest_type digest = {0};
    sha1.get_digest(digest);

    // Check digests
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 808070165U, digest[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 1419762457U, digest[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 664759744U, digest[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 4220766428U, digest[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 2226307254U, digest[4]);
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_with_surview_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "84-CT-Surview";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->get_logger()->count(core::log::log::critical)));
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_mr_with_temporal_position_series()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "85-MR-TemporalPosition";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->get_logger()->count(core::log::log::critical)));
}

//------------------------------------------------------------------------------

void series_set_reader_test::read_ct_series_set_issue01()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "86-CT-Skull";
    const std::filesystem::path path = utest_data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    reader->set_dicom_filter_type("sight::filter::dicom::custom::default_dicom_filter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->get_logger()->count(core::log::log::critical)));
}

} // namespace sight::io::dicom::ut
