/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifdef USE_FW_PYWRAPPING

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/IPyImporter.hpp>

#include "io/IReader.hpp"







using namespace boost::python;

// BOOST_PYTHON_MODULE(io) todo use imp.load or a SCONS define to get this information
BOOST_PYTHON_MODULE(io)
{
	OSLM_INFO("Bundle io exporting python wrapping");
	class_< ::io::IReader, bases< ::fwServices::IService >, ::boost::shared_ptr< ::io::IReader > >("IReader", no_init)
	.def("configureWithIHM", &::io::IReader::configureWithIHM )
	.def("update", &::io::IReader::update   );
}


struct PyImporterForIO : public ::fwTools::IPyImporter
{
	PyImporterForIO() {}
	virtual ~PyImporterForIO() { }

	bool import()
	{
		initio();
		return true;
	}
};


REGISTER_BINDING( ::fwTools::IPyImporter , PyImporterForIO , std::string , "io" )

#endif // USE_FW_PYWRAPPING
