/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_IPATHPOLICY_HPP_
#define _FWXML_POLICY_IPATHPOLICY_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "fwXML/config.hpp"
#include <fwData/Object.hpp>

namespace fwXML
{

/**
 *
 * @brief defines the IPathPolicy class.
 *
 * The purpose of this class is to allow to define which class genererate
 * the path name of an Object.
 *
 */

class FWXML_CLASS_API IPathPolicy
{

public :
    /**
     *
     * @brief   defines a path to save an Object
     *
     * Subclass should implement this method
     *
     * @param obj object to save
     *
     * @return path of the Object
     *
     **/
     FWXML_API virtual boost::filesystem::path getPath( ::boost::shared_ptr < ::fwTools::Object > obj ) = 0 ;

    /**
     * @brief       Virtual destructor
     */
    FWXML_API virtual ~IPathPolicy();

}; //class IPathPolicy

}// namespace fwXML

#endif // _FWXML_POLICY_IPATHPOLICY_HPP_

