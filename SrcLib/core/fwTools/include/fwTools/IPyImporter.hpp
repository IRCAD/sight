/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef IPYIMPORTER_H_
#define IPYIMPORTER_H_

#include <string>
#include "fwTools/config.hpp"

namespace fwTools {

// TODO place this class in fw§Scripting -> rename fwScripting to fwPython, Bundles fwxPyhton -> BPython

/**
 * @brief 	class IPyImporter
 * @class 	IPyImporter
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo 	complete doxygen comments
 */
class FWTOOLS_CLASS_API IPyImporter {
public:
	 FWTOOLS_API IPyImporter();
	 FWTOOLS_API virtual ~IPyImporter();

	 /** @brief Call the py importation.
	  *
	  *  It's code is pretty simple :
	  *  in bundle define pyhton wrapping in  BOOST_PYTHON_MODULE(MyBundleName) { .... }
	  *  then implement a specialized PyImporter with this simple code { initMyBundleName() } thats all.
	  */
	 virtual bool import() = 0;
};

FWTOOLS_API void pyImporterStr(std::string bundleId);



}

#endif /* IPYIMPORTER_H_ */
