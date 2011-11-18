/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_PATIENTFOLDERPATHPOLICY_HPP_
#define _FWXML_POLICY_PATIENTFOLDERPATHPOLICY_HPP_

#include <map>
#include <list>

#include "fwXML/config.hpp"

#include "fwXML/policy/IPathPolicy.hpp"

namespace fwXML
{
/**
 * @brief This class implements the IPathPolicy class.
 *
 * Path : example : P0/S0/A0/R0/reconstruction.xml
 *
 **/
class FWXML_CLASS_API PatientFolderPathPolicy : public IPathPolicy
 {
public :

    fwCoreClassDefinitionsWithFactoryMacro((PatientFolderPathPolicy)(IPathPolicy), ( () ), new PatientFolderPathPolicy );

     /**
     * @brief   defines a path to save an Object
     *
     * path is like this : "P0/S0/A0/R0/reconstruction.xml"
     *
     * @param obj object to save
     *
     * @return path of the Object
     *
     **/
     FWXML_API virtual ::boost::filesystem::path getPath(::fwTools::Object::sptr obj ) ;

 };

}//namespace fwXML

#endif // _FWXML_POLICY_PATIENTFOLDERPATHPOLICY_HPP_

