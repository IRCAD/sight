/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _PREFERENCES_PLUGIN_HPP_
#define _PREFERENCES_PLUGIN_HPP_

#include <string>
#include <boost/filesystem.hpp>

#include <fwRuntime/Plugin.hpp>

#include <fwData/Composite.hpp>

namespace preferences
{

struct Plugin : public ::fwRuntime::Plugin
{
    ~Plugin() throw()
    {}

    void start() throw(::fwRuntime::RuntimeException);

    void stop() throw();

protected:

    ::boost::filesystem::path getPreferencesFile() const;

    static const std::string s_PREF_SERVICE_UID;

    ::fwData::Composite::sptr m_preferences;
};

} // namespace preferences

#endif //_PREFERENCES_PLUGIN_HPP_
