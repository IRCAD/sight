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
 * @brief the Pupose of this class is to manage the sml spliiting in several parts
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLPartitioner // WINSUX : public Singleton< XMLPartitioner >
{
public:

    // WINSUX friend class Singleton< XMLPartitioner >;

    // WINSUX
    FWXML_API static ::boost::shared_ptr< XMLPartitioner > getDefault()
    {
        if ( m_ClassInstance.get() == NULL )
        {
            m_ClassInstance = ::boost::shared_ptr< XMLPartitioner >( new XMLPartitioner );
        }
        return m_ClassInstance;
    }

    FWXML_API virtual  ~XMLPartitioner();

    /// manage the new object son
    /// return a xml pointer node on xml_unit (XMLAggregator) containing son ( not mandatory )
    FWXML_API xmlNodePtr manage( ::boost::shared_ptr< fwTools::Object > father, ::boost::shared_ptr< fwTools::Object > son );
    
    /// allow to change Path policy
    FWXML_API void setPathPolicy( ::boost::shared_ptr< IPathPolicy>  newPathPolicy);
    
    /// allow to change split policy
    FWXML_API void setSplitPolicy( ::boost::shared_ptr< ISplitPolicy>  newSplitPolicy);

protected :

    /// test if object contain extra data (IFileFormat Exist) : if yes update FileFormat.path/dir
    void manageExtraData( ::boost::shared_ptr< fwTools::Object > obj );

    FWXML_API XMLPartitioner();

    std::list< ::boost::shared_ptr< XMLAggregator > > m_aggregators;

    //WINSUX
    FWXML_API static ::boost::shared_ptr< XMLPartitioner > m_ClassInstance;
    
    /// manage file name policy
    ::boost::shared_ptr< IPathPolicy > m_pathPolicy;
    
    /// manage file splitting policy
    ::boost::shared_ptr< ISplitPolicy > m_splitPolicy;
    
    

};

}

#endif /*XMLPARTITIONER_HPP_*/
