/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <wx/menu.h>


#include "fwWX/wxMonitor.hpp"
#include "fwWX/convert.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

wxMonitor::wxMonitor(wxWindow* parent, wxMonitor::MapObjectServicesNamesType & map_string )
 : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxMAXIMIZE_BOX)
{
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    wxSizerFlags flagsExpand(1);
    flagsExpand.Expand();
    wxArrayString strings ;
    wxStaticText* txt1 = new wxStaticText(this, wxID_ANY, _T("Services list :"), wxPoint(20,20), wxDefaultSize );
    sizer->Add( txt1 );
    strings.Add( _T("Show Communications graph") );
    strings.Add( _T("Show Services graph") );
    m_radioBox = new wxRadioBox(this, wxID_ANY, _T("Monitoring"), wxPoint( 20,70 ), wxDefaultSize, strings);
    sizer->Add( m_radioBox ) ;
    m_tree = new wxTreeCtrl(this, wxID_ANY, wxPoint(20,150), wxSize(500, 500), wxTR_DEFAULT_STYLE);
    sizer->Add ( m_tree, flagsExpand ) ;
    this->SetSizer( sizer ) ;
    sizer->SetSizeHints( parent ) ;

    //------------------------------------------------------------------------------
    //---------------------------construct Tree--------------------------------
    std::string string_tmp ="Services :" ;
    wxString itemTreeLabel = ::fwWX::std2wx( string_tmp.data() ) ;
    wxTreeItemId itemPere, itemFils ;
    itemPere = m_tree->AddRoot( itemTreeLabel ) ; // Root= "Racine"

    wxMonitor::MapObjectServicesNamesType::iterator iter_map ;
    std::list<std::string>::iterator iter_list ;

    wxTreeItemId item_with_services = m_tree->AppendItem(m_tree->GetRootItem(), ::fwWX::std2wx("With Services") ) ;
    wxTreeItemId item_without_services = m_tree->AppendItem(m_tree->GetRootItem(), ::fwWX::std2wx("Without Services") ) ;


    for ( iter_map = map_string.begin() ;
        iter_map != map_string.end() ;
        ++iter_map )
    {
        std::stringstream object_name_ss ;
        object_name_ss << (*iter_map).first->className() << "( " << (*iter_map).first << " ) ";

        if ( (*iter_map).second.empty() )
        {
            itemPere = m_tree->AppendItem(item_without_services, ::fwWX::std2wx( object_name_ss.str().data()) ) ;

        }
        else
        {
            assert( ::boost::dynamic_pointer_cast< ::fwData::Object >( (*iter_map).first ) );
            ::boost::shared_ptr< ::fwData::Object > objPtr = ::boost::dynamic_pointer_cast< ::fwData::Object >( (*iter_map).first ) ;
            itemPere = m_tree->AppendItem(item_with_services,
                    ::fwWX::std2wx( object_name_ss.str().data() ),
                    -1,
                    -1,
                    new ObjectPtrItemData( objPtr ) ) ;

            for ( iter_list = (*iter_map).second.begin() ;
                iter_list != (*iter_map).second.end() ;
                ++iter_list )
            {
                itemFils = m_tree->AppendItem(itemPere,
                        ::fwWX::std2wx((*iter_list).data()),
                        -1,
                        -1  ) ;
            }
        }
    }
    //---------------------------END-------construct Tree--------------------------------
    //-----------------------------------------------------------------------------------

    m_tree->ExpandAll() ;
    this->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(wxMonitor::OnSelection)) ;
    this->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxMonitor::OnSelection)) ;
}

//------------------------------------------------------------------------------

wxMonitor::wxMonitor(wxWindow* parent, wxMonitor::string_map & map_string )
 : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxMAXIMIZE_BOX)
{
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    wxSizerFlags flagsExpand(1);
    flagsExpand.Expand();
    m_tree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxSize(500, 500), wxTR_DEFAULT_STYLE);
    sizer->Add(m_tree, flagsExpand);
    SetSizer(sizer);
    //------------------------------------------------------------------------------
    //---------------------------construct Tree--------------------------------
    std::string string_tmp ="Services :" ;
    wxString itemTreeLabel = ::fwWX::std2wx(string_tmp.data()) ;
    wxTreeItemId itemPere, itemFils ;
    itemPere = m_tree->AddRoot( itemTreeLabel ) ;

    wxMonitor::string_map::iterator iter_map ;
    std::list<std::string>::iterator iter_list ;

    wxTreeItemId item_with_services = m_tree->AppendItem(m_tree->GetRootItem(), ::fwWX::std2wx("With Services") ) ;
    wxTreeItemId item_without_services = m_tree->AppendItem(m_tree->GetRootItem(), ::fwWX::std2wx("Without Services") ) ;

    for ( iter_map = map_string.begin() ;
        iter_map != map_string.end() ;
        ++iter_map )
    {
        std::stringstream object_name_ss ;
        object_name_ss << (*iter_map).first ;

        if ( (*iter_map).second.empty() )
        {
            itemPere = m_tree->AppendItem(item_without_services, ::fwWX::std2wx( object_name_ss.str()) ) ;
        }
        else
        {
            itemPere = m_tree->AppendItem(item_with_services, ::fwWX::std2wx( object_name_ss.str() ) ) ;

            for ( iter_list = (*iter_map).second.begin() ;
                iter_list != (*iter_map).second.end() ;
                ++iter_list )
            {
                itemFils = m_tree->AppendItem(itemPere, ::fwWX::std2wx( *iter_list) ) ;
            }
        }
    }
    //---------------------------END-------construct Tree--------------------------------
    //-----------------------------------------------------------------------------------

    m_tree->ExpandAll() ;
}

//------------------------------------------------------------------------------

void wxMonitor::setOnSelectionCallback( wxMonitorCallback::ptr callback)
{
    m_onSelectionCallback = callback;
}

//------------------------------------------------------------------------------

void wxMonitor::OnSelection( wxTreeEvent& event )
{
    //select visually item if different from current item ( event.GetItem() )
    if ( m_tree->GetSelection() != event.GetItem())
    {
        m_tree->SelectItem(event.GetItem()) ;
    }
    //check if current item has an itemData
    if ( m_tree->GetItemData( event.GetItem() ) != NULL )
    {
        ObjectPtrItemData* tmp = dynamic_cast<ObjectPtrItemData*> ( m_tree->GetItemData( event.GetItem() ) ) ;
        if (m_onSelectionCallback)
        {
            (*m_onSelectionCallback.get())(tmp, m_radioBox->GetSelection());
        }
    }
}

//------------------------------------------------------------------------------

wxMonitor::~wxMonitor()
{}

//------------------------------------------------------------------------------

}//namespace fwWX

