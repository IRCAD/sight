/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "preferences/config.hpp"

#include <fwPreferences/IPreferences.hpp>

#include <boost/filesystem/path.hpp>

namespace preferences
{

/**
 * @brief Service to load and save preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::preferences::SPreferences">
       <inout key="preferences" uid="..." />
       <filename>path/to/my/preferences.json</filename>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b preferences [::fwData::Object]: object containing the preferences to load/save.
 * @subsection Configuration Configuration
 * - \b filename : path to the preferences file
 */
class PREFERENCES_CLASS_API SPreferences : public ::fwPreferences::IPreferences
{

public:
    fwCoreServiceClassDefinitionsMacro( (SPreferences)(::fwPreferences::IPreferences) );

    ///Destructor

    SPreferences() noexcept
    {
    }
    virtual ~SPreferences() noexcept
    {
    }

protected:

    /// Starts service and loads preferences if exists.
    PREFERENCES_API virtual void starting() override;

    /// Stops service and saves preferences.
    PREFERENCES_API virtual void stopping() override;

    /// Updates service and saves preferences.
    PREFERENCES_API virtual void updating() override;

    PREFERENCES_API virtual void configuring() override;

    /// preference file
    ::boost::filesystem::path m_prefFile;

private:

    /// loads preferences
    void load();
    /// saves preferences
    void save();
};

} // namespace preferences
