/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwRuntime/operations.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/io/ProfileReader.hpp"
#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/Runtime.hpp"

#include <algorithm>
#include <vector>

namespace fwRuntime
{

namespace
{

//------------------------------------------------------------------------------

/**
 * @brief   Functor that matches configuration element identifiers
 *          against the given identifier
 *
 *
 */
struct ConfigurationElementIdentifierPredicate
{
    ConfigurationElementIdentifierPredicate( const std::string& identifier ) :
        m_identifier( identifier )
    {
    }

    //------------------------------------------------------------------------------

    bool operator() ( std::shared_ptr< ConfigurationElement > element )
    {
        return element->getAttributeValue("id") == m_identifier;
    }

    private:

        std::string m_identifier;

};

}

//------------------------------------------------------------------------------

void init(const boost::filesystem::path& directory)
{
    Runtime* rntm = Runtime::getDefault();
    SLM_ASSERT("Default runtime not found", rntm);

    rntm->setWorkingPath(directory);
    rntm->addDefaultBundles();

}
//------------------------------------------------------------------------------

ConfigurationElement::sptr findConfigurationElement( const std::string& identifier,
                                                     const std::string& pointIdentifier )
{
    typedef std::vector< ConfigurationElement::sptr >  ElementContainer;
    ConfigurationElement::sptr resultConfig;
    ElementContainer elements =
        getAllConfigurationElementsForPoint< ElementContainer >( pointIdentifier );
    ElementContainer::iterator foundElement = ::std::find_if( elements.begin(),
                                                              elements.end(),
                                                              ConfigurationElementIdentifierPredicate(identifier) );
    if(foundElement != elements.end())
    {
        resultConfig = *foundElement;
    }
    return resultConfig;
}

//------------------------------------------------------------------------------

std::shared_ptr< Extension > findExtension( const std::string& identifier )
{
    ::fwRuntime::Runtime* rntm = ::fwRuntime::Runtime::getDefault();
    return rntm->findExtension( identifier );
}

//------------------------------------------------------------------------------

std::shared_ptr< ExtensionPoint > findExtensionPoint(const std::string& identifier)
{
    ::fwRuntime::Runtime* rntm = ::fwRuntime::Runtime::getDefault();
    return rntm->findExtensionPoint( identifier );
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourcePath(const std::string& bundleIdentifier) noexcept
{
    Runtime* rntm                     = Runtime::getDefault();
    std::shared_ptr<Bundle>    bundle = rntm->findBundle( bundleIdentifier );

    if(bundle == nullptr)
    {
        SLM_ERROR("Could not find bundle " + bundleIdentifier + "'");
        return ::boost::filesystem::path();
    }
    return bundle->getResourcesLocation();
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourceFilePath(const std::string& bundleIdentifier,
                                                    const ::boost::filesystem::path& path) noexcept
{
    Runtime* rntm                     = Runtime::getDefault();
    std::shared_ptr<Bundle>    bundle = rntm->findBundle( bundleIdentifier );

    if(bundle == nullptr)
    {
        SLM_ERROR("Could not find bundle '" + bundleIdentifier + "'");
        return ::boost::filesystem::path();
    }
    return getBundleResourcePath(bundle, path);
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourceFilePath(const ::boost::filesystem::path& path) noexcept
{
    SLM_ASSERT("Path should be relative", path.is_relative());
    const std::string bundleIdentifierAndVersion = path.begin()->string();

    // TEMP_FB: Change _ into - when version refactor is made
    auto itVersionDelimiter = bundleIdentifierAndVersion.find(Bundle::s_VERSION_DELIMITER);
    auto bundleIdentifier   = bundleIdentifierAndVersion.substr(0, itVersionDelimiter);
    auto bundleVersion      = bundleIdentifierAndVersion.substr(itVersionDelimiter + 1);

    // Strip the bundle name
    ::boost::filesystem::path pathWithoutBundle;
    for(auto itPath = ++path.begin(); itPath != path.end(); itPath++)
    {
        pathWithoutBundle /= *itPath;
    }

    try
    {
        Runtime* rntm = Runtime::getDefault();
        Version version(bundleVersion);
        std::shared_ptr<Bundle>    bundle = rntm->findBundle( bundleIdentifier, version );

        if(bundle == nullptr)
        {
            SLM_ERROR("Could not find bundle '" + bundleIdentifier + "' with version '" + version.string() + "'");
            return ::boost::filesystem::path();
        }
        return getBundleResourcePath(bundle, pathWithoutBundle );
    }
    catch(...)
    {
        SLM_ERROR("Error looking for bundle '" + bundleIdentifier + "' with version '" + bundleVersion + "'");
        return ::boost::filesystem::path();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path getLibraryResourceFilePath(const ::boost::filesystem::path& path) noexcept
{
    // Currently the library resources are at the same location than bundles
    // This might change in the future
    Runtime* rntm = Runtime::getDefault();
    return (rntm->getWorkingPath() / BUNDLE_RC_PREFIX / path).normalize();
}

//------------------------------------------------------------------------------

::boost::filesystem::path getResourceFilePath(const ::boost::filesystem::path& path) noexcept
{
    auto file = ::fwRuntime::getBundleResourceFilePath(path);
    if(file.empty())
    {
        // If not found in a bundle, look into libraries
        file = ::fwRuntime::getLibraryResourceFilePath(path);
        SLM_ERROR_IF("Resource '" + path.string() + "' has not been found in any bundle or library", file.empty());
    }
    return file;
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourcePath( std::shared_ptr<Bundle> bundle,
                                                 const ::boost::filesystem::path& path) noexcept
{
    return bundle->getResourcesLocation() / path;
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourcePath( ConfigurationElement::sptr element,
                                                 const ::boost::filesystem::path& path) noexcept
{
    return getBundleResourcePath(element->getBundle(), path);
}

//------------------------------------------------------------------------------

::boost::filesystem::path getBundleResourcePath(const IExecutable* executable,
                                                const ::boost::filesystem::path& path) noexcept
{
    return getBundleResourcePath(executable->getBundle(), path);
}

//------------------------------------------------------------------------------

void addBundles( const ::boost::filesystem::path& directory)
{
    Runtime* rntm = Runtime::getDefault();
    SLM_ASSERT("Default runtime not found", rntm);

    rntm->addBundles( directory );
}

//------------------------------------------------------------------------------

std::shared_ptr<Bundle> loadBundle(const std::string& identifier, const Version& version)
{
    Runtime* rntm = Runtime::getDefault();
    SLM_ASSERT("Default runtime not found", rntm);

    auto bundle = ::fwRuntime::Runtime::getDefault()->findBundle(identifier, version);
    if(bundle)
    {
        bundle->setEnable(true);
        bundle->start();
    }

    return bundle;
}

//------------------------------------------------------------------------------

::fwRuntime::profile::Profile::sptr startProfile( const ::boost::filesystem::path& path )
{
    try
    {
        ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::io::ProfileReader::createProfile(path);
        profile->start();
        return profile;
    }
    catch( const std::exception& exception )
    {
        throw RuntimeException( std::string(path.string() + ": invalid profile file. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

std::shared_ptr< Bundle > findBundle( const std::string& identifier, const Version& version )
{
    return Runtime::getDefault()->findBundle( identifier, version );
}

//------------------------------------------------------------------------------

void startBundle(const std::string& identifier)
{
    Runtime* rntm = Runtime::getDefault();

    // Retrieves the specified bundle.
    std::shared_ptr<Bundle> bundle = rntm->findBundle( identifier );
    if( bundle == 0 )
    {
        throw RuntimeException(identifier + ": bundle not found.");
    }
    // Starts the found bundle.
    bundle->start();
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
