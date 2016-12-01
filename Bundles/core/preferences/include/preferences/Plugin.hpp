/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PREFERENCES_PLUGIN_HPP__
#define __PREFERENCES_PLUGIN_HPP__

#include <fwData/Composite.hpp>

#include <fwRuntime/Plugin.hpp>

#include <string>

namespace preferences
{

class Plugin : public ::fwRuntime::Plugin
{
public:
    ~Plugin() throw()
    {
    }

    void start() throw(::fwRuntime::RuntimeException);

    void stop() throw();

protected:

    static const std::string s_PREF_SERVICE_UID;

    ::fwData::Composite::sptr m_preferences;
};

} // namespace preferences

#endif //__PREFERENCES_PLUGIN_HPP__
