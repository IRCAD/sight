/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PREFERENCES_SPREFERENCES_HPP__
#define __PREFERENCES_SPREFERENCES_HPP__

#include "preferences/config.hpp"

#include <fwPreferences/IPreferences.hpp>

#include <boost/filesystem/path.hpp>

namespace preferences
{
/**
 * @brief Service to load and save preferences.
 * @section XML XML Configuration
 * @code{.xml}
         <filename>path/to/my/preferences.json</filename>
 * @endcode
 * @throw ::fwTools::Failed
 */
class PREFERENCES_CLASS_API SPreferences : public ::fwPreferences::IPreferences
{

public:
    fwCoreServiceClassDefinitionsMacro( (SPreferences)(::fwPreferences::IPreferences) );

    ///Destructor

    SPreferences() throw()
    {
    }
    virtual ~SPreferences() throw()
    {
    }

protected:

    /// Starts service and loads preferences if exists.
    PREFERENCES_API virtual void starting() throw( ::fwTools::Failed );

    /// Stops service and saves preferences.
    PREFERENCES_API virtual void stopping() throw( ::fwTools::Failed );

    /// Updates service and saves preferences.
    PREFERENCES_API virtual void updating() throw( ::fwTools::Failed );

    PREFERENCES_API virtual void configuring() throw(::fwTools::Failed);

    /// preference file
    ::boost::filesystem::path m_prefFile;

private:

    /// loads preferences
    void load();
    /// saves preferences
    void save();
};

} // namespace preferences

#endif /*__PREFERENCES_SPREFERENCES_HPP__*/
