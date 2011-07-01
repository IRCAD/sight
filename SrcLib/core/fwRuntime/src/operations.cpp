/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <vector>

#include "fwRuntime/operations.hpp"
#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/io/ProfileReader.hpp"
#include "fwRuntime/profile/Profile.hpp"


namespace fwRuntime
{

namespace
{

//------------------------------------------------------------------------------

/**
 * @brief   Functor that matches configuration element identifiers
 *          against the given identifier
 *
 * @author  IRCAD (Research and Development Team).
 */
struct ConfigurationElementIdentifierPredicate
{
    ConfigurationElementIdentifierPredicate( const std::string & identifier )
    :   m_identifier( identifier )
    {}

    const bool operator() ( ::boost::shared_ptr< ConfigurationElement > element )
    {
        return element->getAttributeValue("id") == m_identifier;
    }

private:

    std::string m_identifier;

};

}

//------------------------------------------------------------------------------

::boost::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string & identifier, const std::string & pointIdentifier )
{
    typedef std::vector< ::boost::shared_ptr< ConfigurationElement > >  ElementContainer;

    ElementContainer            elements = getAllConfigurationElementsForPoint< ElementContainer >( pointIdentifier );
    ElementContainer::iterator  foundElement = ::std::find_if( elements.begin(), elements.end(), ConfigurationElementIdentifierPredicate(identifier) );

    return (foundElement != elements.end()) ? *foundElement : ::boost::shared_ptr< ConfigurationElement >();
}

//------------------------------------------------------------------------------

::boost::shared_ptr< Extension > findExtension( const std::string & identifier )
{
    ::fwRuntime::Runtime* rntm = ::fwRuntime::Runtime::getDefault();
    return rntm->findExtension( identifier );
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ExtensionPoint > findExtensionPoint(const std::string &identifier)
{
    ::fwRuntime::Runtime * rntm = ::fwRuntime::Runtime::getDefault();
    return rntm->findExtensionPoint( identifier );
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path getBundleResourcePath(const std::string& bundleIdentifier, const ::boost::filesystem::path &path) throw()
{
    Runtime *rntm = Runtime::getDefault();
     ::boost::shared_ptr<Bundle>    bundle = rntm->findBundle( bundleIdentifier );
    return bundle != 0 ? getBundleResourcePath(bundle, path) : ::boost::filesystem::path();
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path getBundleResourcePath( ::boost::shared_ptr<Bundle> bundle, const ::boost::filesystem::path &path) throw()
{
    return bundle->getLocation() / path;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path getBundleResourcePath( ::boost::shared_ptr<ConfigurationElement> element, const ::boost::filesystem::path &path) throw()
{
    return getBundleResourcePath(element->getBundle(), path);
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path getBundleResourcePath(const IExecutable *executable, const ::boost::filesystem::path &path) throw()
{
    return getBundleResourcePath(executable->getBundle(), path);
}

//------------------------------------------------------------------------------

void addBundles( const ::boost::filesystem::path & directory) throw(RuntimeException)
{
    Runtime * rntm = Runtime::getDefault();
    rntm->addBundles( directory );
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::profile::Profile > startProfile( const ::boost::filesystem::path & path )
{
    try
    {
        ::boost::shared_ptr< ::fwRuntime::profile::Profile >   profile = ::fwRuntime::io::ProfileReader::createProfile(path);
        profile->start();
        return profile;
    }
    catch( const std::exception & exception )
    {
        throw RuntimeException( std::string(path.string() + ": invalid profile file. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

::boost::shared_ptr< Bundle > findBundle( const std::string & identifier, const Version & version )
{
    return Runtime::getDefault()->findBundle( identifier, version );
}

//------------------------------------------------------------------------------

void startBundle(const std::string &identifier) throw(RuntimeException)
{
    Runtime* rntm = Runtime::getDefault();

    // Retrieves the specified bundle.
    ::boost::shared_ptr<Bundle> bundle = rntm->findBundle( identifier );
    if( bundle == 0 )
    {
        throw RuntimeException(identifier + ": bundle not found.");
    }
    // Starts the found bundle.
    bundle->start();
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
