/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IPYIMPORTER_H_
#define IPYIMPORTER_H_

#include <string>
#include "fwTools/config.hpp"

namespace fwTools {

// TODO place this class in fwScripting -> rename fwScripting to fwPython, Bundles fwxPyhton -> BPython

/**
 * @brief       class IPyImporter is designed as a Interface to allow importing python code in a Bundle.
 * In the Bundle (saying MyBundleName)
 * The developer have to :
 * \li define pyhton wrapping ( for example in  BOOST_PYTHON_MODULE(MyBundleName) \{ .... \} )
 * \li write its own concrete IPyImporter implementing the import method with this line of code { initMyBundleName() }
 * \li register its own concrete IPyImporter using REGISTER_BINDING macro using the bundle name as key
 * @class       IPyImporter
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWTOOLS_CLASS_API IPyImporter {
public:
         FWTOOLS_API IPyImporter();
         FWTOOLS_API virtual ~IPyImporter();

         /** @brief Call the py importation.
          *
          *  It's code (to implement) is pretty simple :
          *  in bundle define pyhton wrapping in  BOOST_PYTHON_MODULE(MyBundleName) { .... }
          *  then implement a specialized PyImporter with this simple code { initMyBundleName() } thats all.
          */
         virtual bool import() = 0;
};

/**
 * @brief this method allow to call the import method concrete IPyImporter
 */
FWTOOLS_API void pyImporterStr(std::string bundleId);



}

#endif /* IPYIMPORTER_H_ */
