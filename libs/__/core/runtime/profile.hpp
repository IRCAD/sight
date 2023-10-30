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

#pragma once

#include "core/config.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <functional>
#include <vector>

namespace sight::core::runtime
{

/**
 * @brief   Implements a module set profile.
 */
class CORE_CLASS_API profile : public core::base_object
{
public:

    using params_container  = std::vector<std::string>;
    using run_callback_type = std::function<int ()>;

    SIGHT_DECLARE_CLASS(profile, base_object);

    CORE_API ~profile() override;

    ///  Starts the profile.
    CORE_API virtual void start() = 0;

    ///  Stops the profile.
    CORE_API virtual void stop() = 0;

    /// Run the profile.
    CORE_API virtual int run() = 0;

    /// Define the callback to be called when running the profile
    CORE_API virtual void set_run_callback(run_callback_type _callback) = 0;

    /// Get profile m_filePath
    std::filesystem::path get_file_path() const
    {
        return m_file_path;
    }

    /// Set profile m_filePath
    void set_file_path(const std::filesystem::path& _file_path)
    {
        m_file_path = _file_path;
    }

    /// Return profile name.
    std::string name() const
    {
        return m_s_name;
    }

    /**
     * @brief       Set profile name.
     *
     * @param[in]   _sName  profile name
     */
    void set_name(std::string _s_name)
    {
        m_s_name = _s_name;
    }

    /**
     * @brief   Return profile version.
     */
    std::string get_version() const
    {
        return m_s_version;
    }

    /**
     * @brief       Set profile version.
     *
     * @param[in]   _sVersion   profile version
     */
    void set_version(std::string _s_version)
    {
        m_s_version = _s_version;
    }

    //------------------------------------------------------------------------------

    params_container get_params() const
    {
        return m_params;
    }

    CORE_API void set_params(const params_container& _params);
    CORE_API void set_params(int _argc, char** _argv);

    /**
     * @brief Returns internal arg count.
     * The returned int shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    int& get_raw_arg_count()
    {
        return m_argc;
    }

    /**
     * @brief Returns a raw pointer on internal arguments.
     * The returned data shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    char** get_raw_params() const
    {
        return m_argv;
    }

protected:

    /**
     * @brief   Constructor : does nothing.
     */
    CORE_API profile();

private:

    std::filesystem::path m_file_path; ///< xml parsed file used to generate profile
    std::string m_s_name;              ///< name profile
    std::string m_s_version;           ///< profile app version

    params_container m_params;
    int m_argc {0};
    char** m_argv {nullptr};
};

/**
 * @brief       Get current profile.
 */
CORE_API core::runtime::profile::sptr get_current_profile();

} // namespace sight::core::runtime
