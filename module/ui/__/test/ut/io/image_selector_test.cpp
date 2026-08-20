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

#include "image_selector_test.hpp"

#include <algorithm>
#include <data/string.hpp>
#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>
#include <service/extension/config.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/dialog/message_base.hpp>
#include <ui/__/dialog/selector_base.hpp>
#include <ui/__/macros.hpp>
#include <ui/test/dialog/location.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/ptree.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::io::ut::image_selector_test);

namespace sight::module::ui::io::ut
{

namespace
{

using filter_t = sight::ui::dialog::location_base::filter_t;

struct test_io_state
{
    std::filesystem::path file;
    std::filesystem::path dialog_file;
    std::size_t dialog_count {0};
    bool should_fail {false};
    bool should_throw {false};
    bool should_drop_file_support {false};
    bool file_support_dropped {false};
};

struct test_selector_state
{
    sight::ui::dialog::selector_base::choices_preset_t choices;
    sight::ui::dialog::selector_base::selections_t selections;
    std::string title;
};

enum class test_service : std::uint8_t
{
    nifti_reader,
    vtk_reader,
    raw_reader,
    folder_reader_a,
    folder_reader_b,
    nifti_writer,
    vtk_writer,
    raw_writer,
    dialog_writer_a,
    dialog_writer_b,
    count
};

} // namespace

//------------------------------------------------------------------------------

static test_io_state& get_state(test_service _service)
{
    static std::array<test_io_state, static_cast<std::size_t>(test_service::count)> s_states;
    return s_states[static_cast<std::size_t>(_service)];
}

//------------------------------------------------------------------------------

static test_selector_state& get_selector_state()
{
    static test_selector_state s_state;
    return s_state;
}

namespace
{

class test_selector final : public sight::ui::dialog::selector_base
{
public:

    //------------------------------------------------------------------------------

    void set_choices_preset(choices_preset_t _choices) override
    {
        get_selector_state().choices = std::move(_choices);
    }

    //------------------------------------------------------------------------------

    void set_title(std::string _title) override
    {
        get_selector_state().title = std::move(_title);
    }

    //------------------------------------------------------------------------------

    void set_multiple(bool /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    selections_t show() override
    {
        return get_selector_state().selections;
    }

    //------------------------------------------------------------------------------

    void set_message(const std::string& /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void add_custom_button(const std::string& /*unused*/, std::function<void()>/*unused*/) override
    {
    }
};

class test_message final : public sight::ui::dialog::message_base
{
public:

    //------------------------------------------------------------------------------

    void set_title(const std::string& /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_message(const std::string& /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_icon(icons /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void add_button(buttons /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_default_button(buttons /*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    void add_custom_button(const std::string& /*unused*/, std::function<void()>/*unused*/) override
    {
    }

    //------------------------------------------------------------------------------

    buttons show() override
    {
        return ok;
    }
};

class test_reader_base : public sight::io::service::reader
{
public:

    test_reader_base(
        const std::string& _description,
        std::vector<std::string> _extensions,
        std::vector<filter_t> _filters,
        test_io_state& _state,
        sight::io::service::path_type_t _path_type = sight::io::service::file
    ) :
        reader(_description),
        m_extensions(std::move(_extensions)),
        m_filters(std::move(_filters)),
        m_state(_state),
        m_path_type(_path_type)
    {
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
        ++m_state.dialog_count;

        if(m_state.should_throw)
        {
            throw std::runtime_error("Reader dialog failure");
        }

        if((m_path_type& sight::io::service::folder) != 0)
        {
            set_folder(m_state.dialog_file);
        }
        else
        {
            set_file(m_state.dialog_file);
        }
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        if(m_state.file_support_dropped)
        {
            return sight::io::service::folder;
        }

        return m_path_type;
    }

    //------------------------------------------------------------------------------

    std::vector<filter_t> get_supported_extensions() override
    {
        if(m_state.should_drop_file_support)
        {
            m_state.file_support_dropped = true;
        }

        if(!m_filters.empty())
        {
            return m_filters;
        }

        std::vector<filter_t> filters;
        filters.reserve(m_extensions.size());
        for(const auto& extension : m_extensions)
        {
            filters.emplace_back(extension, "*" + extension);
        }

        return filters;
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        if(m_state.should_throw)
        {
            throw std::runtime_error("Reader update failure");
        }

        m_state.file  = (m_path_type& sight::io::service::folder) != 0 ? get_folder() : get_file();
        m_read_failed = m_state.should_fail;
    }

private:

    std::vector<std::string> m_extensions;
    std::vector<filter_t> m_filters;
    test_io_state& m_state;
    sight::io::service::path_type_t m_path_type;
};

class test_nifti_reader final : public test_reader_base
{
public:

    SIGHT_DECLARE_SERVICE(test_nifti_reader, sight::io::service::reader);

    test_nifti_reader() :
        test_reader_base(
            "NIfTI reader",
            {".nii", ".nii.gz"},
            {{"NIfTI image", "*.nii *.nii.gz"}},
            get_state(test_service::nifti_reader)
        )
    {
    }
};

class test_vtk_reader final : public test_reader_base
{
public:

    SIGHT_DECLARE_SERVICE(test_vtk_reader, sight::io::service::reader);

    test_vtk_reader() :
        test_reader_base("VTK reader", {".vtk"}, {{"VTK image", "*.vtk"}}, get_state(test_service::vtk_reader))
    {
    }
};

class test_raw_reader final : public test_reader_base
{
public:

    SIGHT_DECLARE_SERVICE(test_raw_reader, sight::io::service::reader);

    test_raw_reader() :
        test_reader_base("Raw reader", {".raw"}, {}, get_state(test_service::raw_reader))
    {
    }
};

class test_folder_reader_a final : public test_reader_base
{
public:

    SIGHT_DECLARE_SERVICE(test_folder_reader_a, sight::io::service::reader);

    test_folder_reader_a() :
        test_reader_base(
            "Folder reader A",
            {},
            {},
            get_state(test_service::folder_reader_a),
            sight::io::service::folder
        )
    {
    }
};

class test_folder_reader_b final : public test_reader_base
{
public:

    SIGHT_DECLARE_SERVICE(test_folder_reader_b, sight::io::service::reader);

    test_folder_reader_b() :
        test_reader_base(
            "Folder reader B",
            {},
            {},
            get_state(test_service::folder_reader_b),
            sight::io::service::folder
        )
    {
    }
};

class test_writer_base : public sight::io::service::writer
{
public:

    test_writer_base(
        const std::string& _description,
        std::vector<std::string> _extensions,
        std::vector<filter_t> _filters,
        test_io_state& _state
    ) :
        writer(_description),
        m_extensions(std::move(_extensions)),
        m_filters(std::move(_filters)),
        m_state(_state)
    {
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
        ++m_state.dialog_count;

        if(m_state.should_throw)
        {
            throw std::runtime_error("Writer dialog failure");
        }

        set_file(m_state.dialog_file);
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::file;
    }

    //------------------------------------------------------------------------------

    std::vector<filter_t> get_supported_extensions() override
    {
        if(!m_filters.empty())
        {
            return m_filters;
        }

        std::vector<filter_t> filters;
        filters.reserve(m_extensions.size());
        for(const auto& extension : m_extensions)
        {
            filters.emplace_back(extension, "*" + extension);
        }

        return filters;
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        if(m_state.should_throw)
        {
            throw std::runtime_error("Writer update failure");
        }

        m_state.file   = get_file();
        m_write_failed = m_state.should_fail;
    }

private:

    std::vector<std::string> m_extensions;
    std::vector<filter_t> m_filters;
    test_io_state& m_state;
};

class test_nifti_writer final : public test_writer_base
{
public:

    SIGHT_DECLARE_SERVICE(test_nifti_writer, sight::io::service::writer);

    test_nifti_writer() :
        test_writer_base(
            "NIfTI writer",
            {".nii", ".nii.gz"},
            {{"NIfTI image", "*.nii *.nii.gz"}},
            get_state(test_service::nifti_writer)
        )
    {
    }
};

class test_vtk_writer final : public test_writer_base
{
public:

    SIGHT_DECLARE_SERVICE(test_vtk_writer, sight::io::service::writer);

    test_vtk_writer() :
        test_writer_base("VTK writer", {".vtk"}, {{"VTK image", "*.vtk"}}, get_state(test_service::vtk_writer))
    {
    }
};

class test_raw_writer final : public test_writer_base
{
public:

    SIGHT_DECLARE_SERVICE(test_raw_writer, sight::io::service::writer);

    test_raw_writer() :
        test_writer_base("Raw writer", {".raw"}, {}, get_state(test_service::raw_writer))
    {
    }
};

class test_dialog_writer_a final : public test_writer_base
{
public:

    SIGHT_DECLARE_SERVICE(test_dialog_writer_a, sight::io::service::writer);

    test_dialog_writer_a() :
        test_writer_base("Dialog writer A", {}, {}, get_state(test_service::dialog_writer_a))
    {
    }
};

class test_dialog_writer_b final : public test_writer_base
{
public:

    SIGHT_DECLARE_SERVICE(test_dialog_writer_b, sight::io::service::writer);

    test_dialog_writer_b() :
        test_writer_base("Dialog writer B", {}, {}, get_state(test_service::dialog_writer_b))
    {
    }
};

} // namespace
SIGHT_REGISTER_GUI(
    sight::module::ui::io::ut::test_selector,
    sight::ui::dialog::selector_base::REGISTRY_KEY
);
SIGHT_REGISTER_GUI(
    sight::module::ui::io::ut::test_message,
    sight::ui::dialog::message_base::REGISTRY_KEY
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::io::ut::test_nifti_reader,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::io::ut::test_vtk_reader,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::io::ut::test_raw_reader,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::io::ut::test_folder_reader_a,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::io::ut::test_folder_reader_b,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::writer,
    sight::module::ui::io::ut::test_nifti_writer,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::writer,
    sight::module::ui::io::ut::test_vtk_writer,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::writer,
    sight::module::ui::io::ut::test_raw_writer,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::writer,
    sight::module::ui::io::ut::test_dialog_writer_a,
    sight::data::string
);
SIGHT_REGISTER_SERVICE(
    sight::io::service::writer,
    sight::module::ui::io::ut::test_dialog_writer_b,
    sight::data::string
);

//------------------------------------------------------------------------------

void image_selector_test::setUp()
{
    get_state(test_service::nifti_reader)    = {};
    get_state(test_service::vtk_reader)      = {};
    get_state(test_service::raw_reader)      = {};
    get_state(test_service::folder_reader_a) = {};
    get_state(test_service::folder_reader_b) = {};
    get_state(test_service::nifti_writer)    = {};
    get_state(test_service::vtk_writer)      = {};
    get_state(test_service::raw_writer)      = {};
    get_state(test_service::dialog_writer_a) = {};
    get_state(test_service::dialog_writer_b) = {};
    get_selector_state()                     = {};

    CPPUNIT_ASSERT(sight::ui::test::dialog::location::clear());

    m_selector = service::add("sight::module::ui::io::selector");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::io::selector'", m_selector);
}

//------------------------------------------------------------------------------

void image_selector_test::tearDown()
{
    if(m_worker)
    {
        m_worker->stop();
        m_worker.reset();
    }

    if(!m_selector->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_selector->stop().get());
    }

    service::remove(m_selector);
    m_selector.reset();
    m_data = nullptr;

    CPPUNIT_ASSERT(sight::ui::test::dialog::location::clear());
}

//------------------------------------------------------------------------------

void image_selector_test::configure_selector(
    const std::string& _data_key,
    const std::vector<std::string>& _service_ids
)
{
    m_data = std::make_shared<sight::data::string>();

    if(_data_key == sight::io::service::READER_DATA_KEY)
    {
        m_selector->set_inout(m_data, sight::io::service::READER_DATA_KEY);
    }
    else if(_data_key == sight::io::service::WRITER_DATA_KEY)
    {
        m_selector->set_input(m_data, sight::io::service::WRITER_DATA_KEY);
    }
    else
    {
        CPPUNIT_FAIL("Unknown selector data key");
    }

    service::config_t config;
    config.put("selection.<xmlattr>.mode", "include");

    for(const auto& service_id : _service_ids)
    {
        boost::property_tree::ptree selection;
        selection.put("<xmlattr>.service", service_id);
        config.add_child("addSelection", selection);
    }

    m_selector->set_config(config);
    CPPUNIT_ASSERT_NO_THROW(m_selector->configure());
    CPPUNIT_ASSERT_NO_THROW(m_selector->start().get());
}

//------------------------------------------------------------------------------

void image_selector_test::update_selector(bool _expect_success)
{
    bool succeeded = false;
    bool failed    = false;

    const auto succeeded_slot = core::com::new_slot(
        [&succeeded]
        {
            succeeded = true;
        });
    const auto failed_slot = core::com::new_slot(
        [&failed]
        {
            failed = true;
        });

    m_worker = core::thread::worker::make();
    succeeded_slot->set_worker(m_worker);
    failed_slot->set_worker(m_worker);
    m_selector->signal("succeeded")->connect(succeeded_slot);
    m_selector->signal("failed")->connect(failed_slot);

    CPPUNIT_ASSERT_NO_THROW(m_selector->update().get());

    SIGHT_TEST_WAIT(succeeded || failed);
    CPPUNIT_ASSERT_EQUAL(_expect_success, succeeded);
    CPPUNIT_ASSERT_EQUAL(!_expect_success, failed);
}

//------------------------------------------------------------------------------

void image_selector_test::file_reader_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});

    // The selected filter determines the reader. The reader remains responsible for interpreting the filename.
    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.data";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::nifti_reader).file);
}

//------------------------------------------------------------------------------

void image_selector_test::multiple_file_readers_test()
{
    configure_selector(
        sight::io::service::READER_DATA_KEY,
        {
            "sight::module::ui::io::ut::test_nifti_reader",
            "sight::module::ui::io::ut::test_vtk_reader"
        });

    const filter_t selected_filter {"VTK image", "*.vtk"};
    sight::ui::test::dialog::location::set_current_filter(selected_filter);

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.data";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    const std::vector<filter_t> expected_filters {
        {"All supported files", "*.nii *.nii.gz *.vtk"},
        {"NIfTI image", "*.nii *.nii.gz"},
        selected_filter
    };
    CPPUNIT_ASSERT(expected_filters == sight::ui::test::dialog::location::get_filters());
    CPPUNIT_ASSERT(get_state(test_service::nifti_reader).file.empty());
    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::vtk_reader).file);
}

//------------------------------------------------------------------------------

void image_selector_test::all_supported_file_readers_test()
{
    configure_selector(
        sight::io::service::READER_DATA_KEY,
        {
            "sight::module::ui::io::ut::test_nifti_reader",
            "sight::module::ui::io::ut::test_vtk_reader"
        });

    const filter_t selected_filter {"All supported files", "*.nii *.nii.gz *.vtk"};
    sight::ui::test::dialog::location::set_current_filter(selected_filter);

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.nii";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    const std::vector<filter_t> expected_filters {
        selected_filter,
        {"NIfTI image", "*.nii *.nii.gz"},
        {"VTK image", "*.vtk"}
    };

    CPPUNIT_ASSERT(expected_filters == sight::ui::test::dialog::location::get_filters());
    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::nifti_reader).file);
    CPPUNIT_ASSERT(get_state(test_service::vtk_reader).file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::reader_without_filter_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_raw_reader"});

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.data";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    const std::vector<filter_t> expected_filters {
        {".raw", "*.raw"},
        {"All supported files", "*.raw"}
    };
    CPPUNIT_ASSERT(expected_filters == sight::ui::test::dialog::location::get_filters());
    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::raw_reader).file);
}

//------------------------------------------------------------------------------

void image_selector_test::unsupported_reader_filter_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});

    sight::ui::test::dialog::location::set_current_filter({"Unsupported", "*.unsupported"});
    sight::ui::test::dialog::location::set_paths({std::filesystem::temp_directory_path() / "image.data"});

    update_selector(false);

    CPPUNIT_ASSERT(get_state(test_service::nifti_reader).file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::reader_failure_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});

    get_state(test_service::nifti_reader).should_fail = true;
    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.nii";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::nifti_reader).file);
}

//------------------------------------------------------------------------------

void image_selector_test::reader_exception_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});

    auto& state = get_state(test_service::nifti_reader);
    state.should_throw = true;

    sight::ui::test::dialog::location::set_paths(
        {std::filesystem::temp_directory_path() / "image.nii"
        });

    update_selector(false);

    CPPUNIT_ASSERT(state.file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::reader_without_file_support_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});

    auto& state = get_state(test_service::nifti_reader);
    state.should_drop_file_support = true;

    sight::ui::test::dialog::location::set_paths(
        {std::filesystem::temp_directory_path() / "image.nii"
        });

    update_selector(false);

    CPPUNIT_ASSERT(state.file.empty());
    CPPUNIT_ASSERT(state.file_support_dropped);
}

//------------------------------------------------------------------------------

void image_selector_test::reader_cancel_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_reader"});
    sight::ui::test::dialog::location::set_paths({});

    update_selector(false);

    CPPUNIT_ASSERT(get_state(test_service::nifti_reader).file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::multiple_folder_readers_test()
{
    const std::string reader_a_id = "sight::module::ui::io::ut::test_folder_reader_a";
    const std::string reader_b_id = "sight::module::ui::io::ut::test_folder_reader_b";
    configure_selector(sight::io::service::READER_DATA_KEY, {reader_a_id, reader_b_id});

    get_selector_state().selections = {reader_b_id};
    const std::filesystem::path selected_folder = std::filesystem::temp_directory_path() / "dicom";
    get_state(test_service::folder_reader_b).dialog_file = selected_folder;

    update_selector(true);

    const sight::ui::dialog::selector_base::choices_preset_t expected_choices {
        {reader_a_id, false},
        {reader_b_id, false}
    };

    auto actual   = get_selector_state().choices;
    auto expected = expected_choices;

    std::ranges::sort(actual);
    std::ranges::sort(expected);

    CPPUNIT_ASSERT(expected == actual);
    CPPUNIT_ASSERT_EQUAL(std::string("Reader to use"), get_selector_state().title);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::folder_reader_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), get_state(test_service::folder_reader_b).dialog_count);
    CPPUNIT_ASSERT_EQUAL(selected_folder, get_state(test_service::folder_reader_b).file);
}

//------------------------------------------------------------------------------

void image_selector_test::folder_reader_cancel_test()
{
    configure_selector(
        sight::io::service::READER_DATA_KEY,
        {
            "sight::module::ui::io::ut::test_folder_reader_a",
            "sight::module::ui::io::ut::test_folder_reader_b"
        });

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::folder_reader_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::folder_reader_b).dialog_count);
}

//------------------------------------------------------------------------------

void image_selector_test::folder_reader_failure_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_folder_reader_a"});

    auto& state = get_state(test_service::folder_reader_a);
    state.dialog_file = std::filesystem::temp_directory_path() / "dicom";
    state.should_fail = true;

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), state.dialog_count);
    CPPUNIT_ASSERT_EQUAL(state.dialog_file, state.file);
}

//------------------------------------------------------------------------------

void image_selector_test::folder_reader_exception_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::test_folder_reader_a"});

    auto& state = get_state(test_service::folder_reader_a);
    state.dialog_file  = std::filesystem::temp_directory_path() / "dicom";
    state.should_throw = true;

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), state.dialog_count);
    CPPUNIT_ASSERT(state.file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::multiple_file_writers_test()
{
    configure_selector(
        sight::io::service::WRITER_DATA_KEY,
        {
            "sight::module::ui::io::ut::test_nifti_writer",
            "sight::module::ui::io::ut::test_vtk_writer"
        });

    const filter_t selected_filter {"VTK image", "*.vtk"};
    sight::ui::test::dialog::location::set_current_filter(selected_filter);

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.vtk";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    const std::vector<filter_t> expected_filters {
        {"NIfTI image", "*.nii *.nii.gz"},
        selected_filter
    };
    CPPUNIT_ASSERT(expected_filters == sight::ui::test::dialog::location::get_filters());
    CPPUNIT_ASSERT(get_state(test_service::nifti_writer).file.empty());
    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::vtk_writer).file);
}

//------------------------------------------------------------------------------

void image_selector_test::writer_extension_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    CPPUNIT_ASSERT_EQUAL(
        std::filesystem::path(selected_file.string() + ".nii"),
        get_state(test_service::nifti_writer).file
    );
}

//------------------------------------------------------------------------------

void image_selector_test::writer_compound_extension_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.nii.gz";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::nifti_writer).file);
}

//------------------------------------------------------------------------------

void image_selector_test::writer_without_filter_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_raw_writer"});

    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    const std::vector<filter_t> expected_filters {{".raw", "*.raw"}};
    CPPUNIT_ASSERT(expected_filters == sight::ui::test::dialog::location::get_filters());
    CPPUNIT_ASSERT_EQUAL(
        std::filesystem::path(selected_file.string() + ".raw"),
        get_state(test_service::raw_writer).file
    );
}

//------------------------------------------------------------------------------

void image_selector_test::unsupported_writer_filter_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});

    sight::ui::test::dialog::location::set_current_filter({"Unsupported", "*.unsupported"});
    sight::ui::test::dialog::location::set_paths({std::filesystem::temp_directory_path() / "image"});

    update_selector(false);

    CPPUNIT_ASSERT(get_state(test_service::nifti_writer).file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::writer_failure_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});

    get_state(test_service::nifti_writer).should_fail = true;
    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.nii";
    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::nifti_writer).file);
}

//------------------------------------------------------------------------------

void image_selector_test::writer_exception_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});

    auto& state = get_state(test_service::nifti_writer);
    state.should_throw = true;

    sight::ui::test::dialog::location::set_paths(
        {std::filesystem::temp_directory_path() / "image.nii"
        });

    update_selector(false);

    CPPUNIT_ASSERT(state.file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::writer_cancel_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_nifti_writer"});
    sight::ui::test::dialog::location::set_paths({});

    update_selector(false);

    CPPUNIT_ASSERT(get_state(test_service::nifti_writer).file.empty());
}

//------------------------------------------------------------------------------

void image_selector_test::multiple_dialog_writers_test()
{
    const std::string writer_a_id = "sight::module::ui::io::ut::test_dialog_writer_a";
    const std::string writer_b_id = "sight::module::ui::io::ut::test_dialog_writer_b";
    configure_selector(sight::io::service::WRITER_DATA_KEY, {writer_a_id, writer_b_id});

    get_selector_state().selections = {writer_b_id};
    const std::filesystem::path selected_file = std::filesystem::temp_directory_path() / "image.custom";
    get_state(test_service::dialog_writer_b).dialog_file = selected_file;

    update_selector(true);

    const sight::ui::dialog::selector_base::choices_preset_t expected_choices {
        {writer_a_id, false},
        {writer_b_id, false}
    };
    auto actual   = get_selector_state().choices;
    auto expected = expected_choices;

    std::ranges::sort(actual);
    std::ranges::sort(expected);

    CPPUNIT_ASSERT(expected == actual);

    CPPUNIT_ASSERT_EQUAL(std::string("Writer to use"), get_selector_state().title);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::dialog_writer_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), get_state(test_service::dialog_writer_b).dialog_count);
    CPPUNIT_ASSERT_EQUAL(selected_file, get_state(test_service::dialog_writer_b).file);
}

//------------------------------------------------------------------------------

void image_selector_test::dialog_writer_cancel_test()
{
    configure_selector(
        sight::io::service::WRITER_DATA_KEY,
        {
            "sight::module::ui::io::ut::test_dialog_writer_a",
            "sight::module::ui::io::ut::test_dialog_writer_b"
        });

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::dialog_writer_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::dialog_writer_b).dialog_count);
}

//------------------------------------------------------------------------------

void image_selector_test::dialog_writer_failure_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_dialog_writer_a"});

    auto& state = get_state(test_service::dialog_writer_a);
    state.dialog_file = std::filesystem::temp_directory_path() / "image.custom";
    state.should_fail = true;

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), state.dialog_count);
    CPPUNIT_ASSERT_EQUAL(state.dialog_file, state.file);
}

//------------------------------------------------------------------------------

void image_selector_test::dialog_writer_exception_test()
{
    configure_selector(sight::io::service::WRITER_DATA_KEY, {"sight::module::ui::io::ut::test_dialog_writer_a"});

    auto& state = get_state(test_service::dialog_writer_a);
    state.dialog_file  = std::filesystem::temp_directory_path() / "image.custom";
    state.should_throw = true;

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), state.dialog_count);
    CPPUNIT_ASSERT(state.file.empty());
}

//------------------------------------------------------------------------------
void image_selector_test::unknown_folder_reader_selection_test()
{
    const std::string reader_a_id = "sight::module::ui::io::ut::test_folder_reader_a";
    const std::string reader_b_id = "sight::module::ui::io::ut::test_folder_reader_b";

    configure_selector(sight::io::service::READER_DATA_KEY, {reader_a_id, reader_b_id});

    get_selector_state().selections = {"unknown_reader"};

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::folder_reader_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::folder_reader_b).dialog_count);
}

//------------------------------------------------------------------------------
void image_selector_test::unknown_dialog_writer_selection_test()
{
    const std::string writer_a_id = "sight::module::ui::io::ut::test_dialog_writer_a";
    const std::string writer_b_id = "sight::module::ui::io::ut::test_dialog_writer_b";

    configure_selector(sight::io::service::WRITER_DATA_KEY, {writer_a_id, writer_b_id});

    get_selector_state().selections = {"unknown_writer"};

    update_selector(false);

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::dialog_writer_a).dialog_count);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), get_state(test_service::dialog_writer_b).dialog_count);
}

//------------------------------------------------------------------------------

void image_selector_test::writer_with_config_test()
{
    const std::string writer_id = "sight::module::ui::io::ut::test_vtk_writer";
    const std::string config_id = "image_selector_test_writer_config";

    service::config_t writer_config;
    writer_config.put("dummy", "value");

    service::extension::config::get_default()->add_service_config_info(
        config_id,
        writer_id,
        "Test writer config",
        writer_config
    );

    m_data = std::make_shared<sight::data::string>();
    m_selector->set_input(m_data, sight::io::service::WRITER_DATA_KEY);

    service::config_t config;
    config.put("selection.<xmlattr>.mode", "include");

    boost::property_tree::ptree selection;
    selection.put("<xmlattr>.service", writer_id);
    selection.put("<xmlattr>.config", config_id);
    config.add_child("addSelection", selection);

    m_selector->set_config(config);

    CPPUNIT_ASSERT_NO_THROW(m_selector->configure());
    CPPUNIT_ASSERT_NO_THROW(m_selector->start().get());

    const std::filesystem::path selected_file =
        std::filesystem::temp_directory_path() / "image.vtk";

    sight::ui::test::dialog::location::set_paths({selected_file});

    update_selector(true);

    CPPUNIT_ASSERT_EQUAL(
        selected_file,
        get_state(test_service::vtk_writer).file
    );
}

//------------------------------------------------------------------------------
void image_selector_test::no_available_service_test()
{
    configure_selector(sight::io::service::READER_DATA_KEY, {"sight::module::ui::io::ut::unknown_reader"});

    update_selector(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io::ut
