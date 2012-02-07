/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLPARTITIONER_HPP_
#define XMLPARTITIONER_HPP_

#include <libxml/tree.h>
#include <list>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"
#include "fwXML/XML/XMLAggregator.hpp"
#include "fwXML/policy/ISplitPolicy.hpp"
#include "fwXML/policy/IPathPolicy.hpp"

namespace fwXML
{

/**
 * @brief the Purpose of this class is to manage the xml splitting in several parts
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLPartitioner : public ::fwCore::BaseObject
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (XMLPartitioner)(::fwCore::BaseObject), (()), new XMLPartitioner );

    FWXML_API static XMLPartitioner::sptr getDefault()
    {
        if ( m_ClassInstance.get() == NULL )
        {
            m_ClassInstance = XMLPartitioner::NewSptr();
        }
        return m_ClassInstance;
    }

    FWXML_API virtual  ~XMLPartitioner();

    /// manage the new object son
    /// return a xml pointer node on xml_unit (XMLAggregator) containing son ( not mandatory )
    FWXML_API xmlNodePtr manage( ::fwTools::Object::sptr father, ::fwTools::Object::sptr son );

    /// allow to change Path policy
    FWXML_API void setPathPolicy( IPathPolicy::sptr newPathPolicy);

    /// allow to change split policy
    FWXML_API void setSplitPolicy( ISplitPolicy::sptr newSplitPolicy);

protected :

    /// test if object contain extra data (IFileFormat Exist) : if yes update FileFormat.path/dir
    void manageExtraData( ::fwTools::Object::sptr obj );

    FWXML_API XMLPartitioner();

    std::list< XMLAggregator::sptr > m_aggregators;

    //WINSUX
    FWXML_API static XMLPartitioner::sptr m_ClassInstance;

    /// manage file name policy
    IPathPolicy::sptr m_pathPolicy;

    /// manage file splitting policy
    ISplitPolicy::sptr m_splitPolicy;
};

}

#endif /*XMLPARTITIONER_HPP_*/
