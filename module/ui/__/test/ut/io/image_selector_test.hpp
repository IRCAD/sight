/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#pragma once

#include <core/thread/worker.hpp>

#include <data/string.hpp>

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::io::ut
{

class image_selector_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_selector_test);
CPPUNIT_TEST(file_reader_test);
CPPUNIT_TEST(multiple_file_readers_test);
CPPUNIT_TEST(all_supported_file_readers_test);
CPPUNIT_TEST(reader_without_filter_test);
CPPUNIT_TEST(unsupported_reader_filter_test);
CPPUNIT_TEST(reader_failure_test);
CPPUNIT_TEST(reader_exception_test);
CPPUNIT_TEST(reader_without_file_support_test);
CPPUNIT_TEST(reader_cancel_test);
CPPUNIT_TEST(multiple_folder_readers_test);
CPPUNIT_TEST(folder_reader_cancel_test);
CPPUNIT_TEST(folder_reader_failure_test);
CPPUNIT_TEST(folder_reader_exception_test);
CPPUNIT_TEST(multiple_file_writers_test);
CPPUNIT_TEST(writer_extension_test);
CPPUNIT_TEST(writer_compound_extension_test);
CPPUNIT_TEST(writer_without_filter_test);
CPPUNIT_TEST(unsupported_writer_filter_test);
CPPUNIT_TEST(writer_failure_test);
CPPUNIT_TEST(writer_exception_test);
CPPUNIT_TEST(writer_cancel_test);
CPPUNIT_TEST(multiple_dialog_writers_test);
CPPUNIT_TEST(dialog_writer_cancel_test);
CPPUNIT_TEST(dialog_writer_failure_test);
CPPUNIT_TEST(dialog_writer_exception_test);
CPPUNIT_TEST(no_available_service_test);
CPPUNIT_TEST(unknown_folder_reader_selection_test);
CPPUNIT_TEST(unknown_dialog_writer_selection_test);
CPPUNIT_TEST(writer_with_config_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void file_reader_test();
    void multiple_file_readers_test();
    void all_supported_file_readers_test();
    void reader_without_filter_test();
    void unsupported_reader_filter_test();
    void reader_failure_test();
    void reader_exception_test();
    void reader_without_file_support_test();
    void reader_cancel_test();
    void multiple_folder_readers_test();
    void folder_reader_cancel_test();
    void folder_reader_failure_test();
    void folder_reader_exception_test();
    void multiple_file_writers_test();
    void writer_extension_test();
    void writer_compound_extension_test();
    void writer_without_filter_test();
    void unsupported_writer_filter_test();
    void writer_failure_test();
    void writer_exception_test();
    void writer_cancel_test();
    void multiple_dialog_writers_test();
    void dialog_writer_cancel_test();
    void dialog_writer_failure_test();
    void dialog_writer_exception_test();
    void no_available_service_test();
    void unknown_folder_reader_selection_test();
    void unknown_dialog_writer_selection_test();
    void writer_with_config_test();

private:

    void configure_selector(
        const std::string& _data_key,
        const std::vector<std::string>& _service_ids
    );
    void update_selector(bool _expect_success);

    service::base::sptr m_selector;
    core::thread::worker::sptr m_worker;
    data::string::sptr m_data;
};

} // namespace sight::module::ui::io::ut
