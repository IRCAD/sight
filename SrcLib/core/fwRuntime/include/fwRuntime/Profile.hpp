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

#pragma once

#include "fwRuntime/config.hpp"

#include <fwCore/base.hpp>

#include <filesystem>
#include <functional>
#include <vector>

namespace fwRuntime
{

/**
 * @brief   Implements a module set profile.
 */
class Profile : public ::fwCore::BaseObject
{
public:

    typedef std::vector< std::string >  ParamsContainer;
    typedef std::function< int () > RunCallbackType;

    fwCoreClassMacro(Profile, BaseObject)

    /**
     * @brief Profile factory.
     */
    static sptr New();

    FWRUNTIME_API ~Profile() override;

    ///  Starts the profile.
    FWRUNTIME_API virtual void start() = 0;

    ///  Stops the profile.
    FWRUNTIME_API virtual void stop() = 0;

    /// Setup the profile
    FWRUNTIME_API virtual void setup() = 0;

    /// Cleanup the profile
    FWRUNTIME_API virtual void cleanup() = 0;

    /// Run the profile.
    FWRUNTIME_API virtual int run() = 0;

    /// Define the callback to be called when running the profile
    FWRUNTIME_API virtual void setRunCallback(RunCallbackType callback) = 0;

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

    /**
     * @brief   Return profile CheckSingleInstance.
     */
    bool getCheckSingleInstance() const
    {
        return m_checkSingleInstance;
    }

    /**
     * @brief       Set profile CheckSingleInstance.
     *
     * @param[in]   _checkSingleInstance    profile CheckSingleInstance
     */
    void setCheckSingleInstance(bool _checkSingleInstance)
    {
        m_checkSingleInstance = _checkSingleInstance;
    }

    //------------------------------------------------------------------------------

    ParamsContainer getParams() const
    {
        return m_params;
    }

    FWRUNTIME_API void setParams(const ParamsContainer& params);
    FWRUNTIME_API void setParams(int argc, char** argv);

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
    FWRUNTIME_API Profile();

private:

    std::string m_sName;                    ///< name profile
    std::string m_sVersion;                 ///< profile app version
    std::filesystem::path m_filePath;   ///< xml parsed file used to generate profile

    bool m_checkSingleInstance;

    ParamsContainer m_params;
    int m_argc;
    char**          m_argv;
};

/**
 * @brief       Set current profile.
 *
 * @param       prof profile
 */
FWRUNTIME_API void setCurrentProfile(::fwRuntime::Profile::sptr prof);

/**
 * @brief       Get current profile.
 */
FWRUNTIME_API ::fwRuntime::Profile::sptr getCurrentProfile();

} // namespace fwRuntime
