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
class Profile : public core::BaseObject
{
public:

    typedef std::vector<std::string> ParamsContainer;
    typedef std::function<int ()> RunCallbackType;

    SIGHT_DECLARE_CLASS(Profile, BaseObject);

    /**
     * @brief Profile factory.
     */
    static sptr New();

    CORE_API ~Profile() override;

    ///  Starts the profile.
    CORE_API virtual void start() = 0;

    ///  Stops the profile.
    CORE_API virtual void stop() = 0;

    /// Run the profile.
    CORE_API virtual int run() = 0;

    /// Define the callback to be called when running the profile
    CORE_API virtual void setRunCallback(RunCallbackType callback) = 0;

    /// Get profile m_filePath
    std::filesystem::path getFilePath() const
    {
        return m_filePath;
    }

    /// Set profile m_filePath
    void setFilePath(const std::filesystem::path& _filePath)
    {
        m_filePath = _filePath;
    }

    /// Return profile name.
    std::string getName() const
    {
        return m_sName;
    }

    /**
     * @brief       Set profile name.
     *
     * @param[in]   _sName  profile name
     */
    void setName(std::string _sName)
    {
        m_sName = _sName;
    }

    /**
     * @brief   Return profile version.
     */
    std::string getVersion() const
    {
        return m_sVersion;
    }

    /**
     * @brief       Set profile version.
     *
     * @param[in]   _sVersion   profile version
     */
    void setVersion(std::string _sVersion)
    {
        m_sVersion = _sVersion;
    }

    //------------------------------------------------------------------------------

    ParamsContainer getParams() const
    {
        return m_params;
    }

    CORE_API void setParams(const ParamsContainer& params);
    CORE_API void setParams(int argc, char** argv);

    /**
     * @brief Returns internal arg count.
     * The returned int shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    int& getRawArgCount()
    {
        return m_argc;
    }

    /**
     * @brief Returns a raw pointer on internal arguments.
     * The returned data shall not be modified. This is provided for external
     * library needs (QApplication contructor for example)
     */
    char** getRawParams() const
    {
        return m_argv;
    }

protected:

    /**
     * @brief   Constructor : does nothing.
     */
    CORE_API Profile();

private:

    std::filesystem::path m_filePath; ///< xml parsed file used to generate profile
    std::string m_sName;              ///< name profile
    std::string m_sVersion;           ///< profile app version

    ParamsContainer m_params;
    int m_argc {0};
    char** m_argv {nullptr};
};

/**
 * @brief       Set current profile.
 * @deprecated  This should not be called outside this library and will become part of private API as of 22.0
 * @param       prof profile
 */
[[deprecated("To be removed in Sight 22.0, profile is set by runtime internally.")]]
CORE_API void setCurrentProfile(core::runtime::Profile::sptr prof);

/**
 * @brief       Get current profile.
 */
CORE_API core::runtime::Profile::sptr getCurrentProfile();

} // namespace sight::core::runtime
