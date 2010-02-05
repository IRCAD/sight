/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/IPyImporter.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"

namespace fwTools {

IPyImporter::IPyImporter() {
        // TODO Auto-generated constructor stub

}

IPyImporter::~IPyImporter() {
        // TODO Auto-generated destructor stub
}



void pyImporterStr(std::string bundleId)
{
        ::boost::shared_ptr< ::fwTools::IPyImporter > pyImporter;
        pyImporter = ::fwTools::ClassFactoryRegistry::create< ::fwTools::IPyImporter >( bundleId );
        if ( pyImporter )
        {
                OSLM_WARN( "EXE PyImporter for " <<  bundleId );
                pyImporter->import();
        }
        else
        {
                OSLM_WARN( "No PyImporter for " << bundleId );
        }
}



}
