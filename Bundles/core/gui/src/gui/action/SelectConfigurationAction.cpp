/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>

#include <fwCore/base.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>


#include "gui/action/SelectConfigurationAction.hpp"

namespace gui
{

namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::SelectConfigurationAction , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

class Selector :  public wxDialog
{
public:

    /**
     * @brief   Constructor
     * 
     * @param   parent  a pointer to a window to use as the parent window
     * 
     */
    Selector( wxWindow * parent , std::string _title , std::vector< wxString > _selections)
    :   wxDialog    ( parent, -1, wxString(  wxConvertMB2WX(_title.c_str()) ), wxDefaultPosition, 
                      wxDefaultSize, 
                      wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX),
                      m_selections( _selections )
    {
        // Creates the static fields.
        m_typeCtrl = new wxComboBox( this, -1 , wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN|wxCB_READONLY );
        // Initialize combo box
        std::vector< wxString >::iterator iter ;
        for ( iter = m_selections.begin() ; iter != m_selections.end() ; ++iter ) 
        {
            m_typeCtrl->Append( *iter );
            m_typeCtrl->SetValue( *iter );
        }
        
        // Creates the default buttons.
        wxSizer     * defaultButtonSizer = new wxBoxSizer( wxHORIZONTAL );
        wxButton    * okButton = new wxButton( this, wxID_OK, _T("OK") );
        wxButton    * cancelButton = new wxButton( this, wxID_CANCEL, _T("Cancel") );
        
        okButton->SetDefault();
        defaultButtonSizer->Add( okButton, 0, 0 );
        defaultButtonSizer->Add( 5, 5 );
        defaultButtonSizer->Add( cancelButton, 0, 0 );

        // Creates the root sizer.
        wxSizer * rootSizer = new wxBoxSizer( wxVERTICAL );

    //  rootSizer->Add( typeText, 0, wxGROW|wxALL, 10 );
        rootSizer->Add( m_typeCtrl, 0, wxGROW|wxALL, 10 );
        
        rootSizer->Add( defaultButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
        SetSizerAndFit( rootSizer );
    }
    
    /// editor Fields
    wxComboBox *m_typeCtrl;
    /// Number of index
    std::vector< wxString >     m_selections ;
};

//-----------------------------------------------------------------------------

SelectConfigurationAction::SelectConfigurationAction() throw()
{
}

//-----------------------------------------------------------------------------

SelectConfigurationAction::~SelectConfigurationAction() throw() 
{
}

//-----------------------------------------------------------------------------

void SelectConfigurationAction::info(std::ostream &_sstream )
{
    _sstream << "SelectConfiguration Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SelectConfigurationAction::updating() throw( ::fwTools::Failed )
{
    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > extensions = ::fwServices::bundle::findExtensionsForPoint( "::fwTools::Object" ) ;
    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator itertruc ;
    typedef std::map< std::string , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > > CfgMapTypePerObjectType ;
    typedef std::map< std::string ,  CfgMapTypePerObjectType > ObjectToConfigContainerMapType;
    ObjectToConfigContainerMapType db ;
    for( itertruc = extensions.begin() ; itertruc != extensions.end() ; ++itertruc )
    {
        if( ::fwRuntime::findExtensionPoint( (*itertruc)->getIdentifier() ) )
        {
            CfgMapTypePerObjectType association = ::fwRuntime::getAllIdAndConfigurationElementsForPoint( (*itertruc)->getIdentifier() ) ;
            db[(*itertruc)->getIdentifier()] = association ;
        }
    }
    
    std::vector< wxString > wxIdsForObjectType ;
    ObjectToConfigContainerMapType::iterator iter2 ;
    for( iter2 = db.begin() ; iter2 != db.end() ; ++iter2 ) 
    {
        SLM_TRACE( iter2->first );
        wxIdsForObjectType.push_back( wxString( wxConvertMB2WX(iter2->first.c_str()) ) ) ;
    }   
    
    // Selecting the object type configuration  
    Selector selectorForObjectType( wxTheApp->GetTopWindow() , "Object Type" , wxIdsForObjectType );
    if( selectorForObjectType.ShowModal() == wxID_OK )
    {
        std::string objectTypeId(  wxConvertWX2MB( selectorForObjectType.m_typeCtrl->GetValue() ) );
        std::vector< wxString > wxIdsForConfig ;
        CfgMapTypePerObjectType::iterator iter2 ;
        for( iter2 = db[objectTypeId].begin() ; iter2 != db[objectTypeId].end() ; ++iter2 ) 
        {
            wxIdsForConfig.push_back( wxString( wxConvertMB2WX(iter2->first.c_str()) ) ) ;
        }
        Selector selectorForConfig( wxTheApp->GetTopWindow() , "Configuration" , wxIdsForConfig );
        if( selectorForConfig.ShowModal() == wxID_OK )
        {
            std::string configId(  wxConvertWX2MB( selectorForConfig.m_typeCtrl->GetValue() ) );
            ::fwServices::OSR::uninitializeRootObject();
            ::fwServices::OSR::setRootObjectClassName(objectTypeId) ;
            ::fwServices::OSR::setRootObjectConfigurationName(configId) ;
            ::fwServices::OSR::initializeRootObject();
        }
    }
}
}
}
