/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_LISTMANAGEDPOLICY_H_
#define _FWXML_POLICY_LISTMANAGEDPOLICY_H_

#include <set>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

#include "fwXML/policy/ISplitPolicy.hpp"

namespace fwXML
{
/**
 * @brief   defines the AlwaysSplitPolicy class.
 */
class FWXML_CLASS_API ListManagedPolicy : public ISplitPolicy
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((ListManagedPolicy)(ISplitPolicy), ( () ), new ListManagedPolicy );

    /**
     * @brief   true if the object should be spliten, false otherwise
     */
    FWXML_API virtual bool split( ::fwTools::Object::sptr obj) ;

    /**
     * @brief   add a class to ListManagedPolicy
     * @param   className name of the class, given by className()
     */
    FWXML_API void addSplitClass( const std::string &className);


private :

    typedef std::set< std::string > ClassNameContainer;

    /**
     * @brief store class name which require a splitting.
     * Bu default empty list
     */
    ClassNameContainer m_classNames;
};

} //namespace fwXML

#endif // _FWXML_POLICY_LISTMANAGEDPOLICY_H_
