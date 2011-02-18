/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwCore/base.hpp>

#include "fwXML/ImageFileFormatService.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"
#include "fwXML/XML/XMLHierarchy.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
// default policies
#include "fwXML/policy/AlwaysSplitPolicy.hpp"
#include "fwXML/policy/DefaultPathPolicy.hpp"


::boost::shared_ptr< fwXML::XMLPartitioner > fwXML::XMLPartitioner::m_ClassInstance = boost::shared_ptr< fwXML::XMLPartitioner >();


namespace fwXML
{

//------------------------------------------------------------------------------

XMLPartitioner::XMLPartitioner()
: m_pathPolicy ( new DefaultPathPolicy ),
  m_splitPolicy( new AlwaysSplitPolicy )
{
    assert( m_pathPolicy  );
    assert( m_splitPolicy );
}

//------------------------------------------------------------------------------

XMLPartitioner::~XMLPartitioner()
{}

//------------------------------------------------------------------------------

void XMLPartitioner::setPathPolicy( ::boost::shared_ptr< IPathPolicy>  newPathPolicy)
{
    SLM_DEBUG("Changing path policy");
    m_pathPolicy=newPathPolicy;
}

//------------------------------------------------------------------------------

void XMLPartitioner::setSplitPolicy( ::boost::shared_ptr< ISplitPolicy>  newSplitPolicy)
{
    SLM_DEBUG("Changing split policy");
    m_splitPolicy=newSplitPolicy;
}

//------------------------------------------------------------------------------

::boost::filesystem::path DefaultRoot()
{
#ifdef WIN32
    ::boost::filesystem::path root( "C:\\tmp" );
#else
    ::boost::filesystem::path root( "/tmp" );
#endif

     return root;
}

//------------------------------------------------------------------------------

// test if object manage extra data aka FileFormat
// no  : do nothing
// yes : call FileFormat and set path according to its aggregator
// precondition object already in an aggregator

// saver->directory the same as the aggregator

void XMLPartitioner::manageExtraData( ::fwTools::Object::sptr obj )
{
    if ( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),  "::fwXML::IFileFormatService") )
    {
        ::fwXML::IFileFormatService::sptr  saver = ::fwServices::get< ::fwXML::IFileFormatService >(obj);

        if (saver)
        {
            ::boost::shared_ptr< XMLAggregator > aggregator =  XMLHierarchy::getDefault()->mapObjectAggregator()[obj];
            saver->rootFolder()  = aggregator->rootFolder();
            saver->localFolder() = aggregator->localFolder();
            saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj);
            saver->extension() = saver->getWriter()->extension();
            SLM_DEBUG("update path");
        }
    }
}

//------------------------------------------------------------------------------

xmlNodePtr XMLPartitioner::manage( ::fwTools::Object::sptr father, ::fwTools::Object::sptr  son )
{
    XMLHierarchy::getDefault()->mapChildFather()[son]=father;
    XMLHierarchy::getDefault()->mapFatherChildren()[father].insert(son);

    if (father.get()==NULL )
    {
        assert( XMLHierarchy::getDefault()->mapObjectAggregator()[son].get()==NULL );
        // root serialization : create a new Aggregator
        XMLAggregator::NewSptr newAggregator;

        newAggregator->rootFolder()  =   DefaultRoot();
        newAggregator->localFolder() =  m_pathPolicy->getPath(son).branch_path();
        newAggregator->filename()    = ::boost::filesystem::basename( m_pathPolicy->getPath(son).leaf() ) ;
        newAggregator->extension()   = ::boost::filesystem::extension( m_pathPolicy->getPath(son).leaf() );
        newAggregator->append(son);
        XMLHierarchy::getDefault()->mapObjectAggregator()[son]=newAggregator;

        // delegate extra XML serialization information if necessary
        manageExtraData(son);
        return NULL;
    }

    bool splitXML = m_splitPolicy->split( son );

    if  (splitXML==false)
    {
        // no splitting :  append in the same serialization unit of its father
        ::fwXML::XMLAggregator::sptr fatherAggregator =  XMLHierarchy::getDefault()->mapObjectAggregator()[father];

        assert( fatherAggregator );
        fatherAggregator->append( son );

        // record son'as aggregator
        XMLHierarchy::getDefault()->mapObjectAggregator()[son]=fatherAggregator;

        // delegate extra XML serialization information if necessary
        manageExtraData(son);

        //xmlSaveFile( "-" , fatherAggregator->getXMLDoc() );
        return NULL;
    }
    else
    {
        // splitting requited : create a new Aggregator
        XMLAggregator::NewSptr newAggregator;

        // FSLocation
        newAggregator->rootFolder()  =   DefaultRoot();
        newAggregator->localFolder() =  m_pathPolicy->getPath(son).branch_path();
        newAggregator->filename()    = ::boost::filesystem::basename( m_pathPolicy->getPath(son).leaf() ) ;
        newAggregator->extension()   = ::boost::filesystem::extension( m_pathPolicy->getPath(son).leaf() );

        newAggregator->append(son);
        XMLHierarchy::getDefault()->mapObjectAggregator()[son]=newAggregator;
        //xmlSaveFile( "-" , newAggregator->getXMLDoc() );

        // delegate extra XML serialization information if necessary
        manageExtraData(son);

        return NULL;
    }
}

//------------------------------------------------------------------------------

}
