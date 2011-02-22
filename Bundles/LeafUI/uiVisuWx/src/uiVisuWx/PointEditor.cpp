/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <wx/window.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/string.h>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include "fwData/Composite.hpp"
#include "fwData/String.hpp"

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>

#include <fwComEd/InteractionMsg.hpp>

#include <fwGuiWx/container/WxContainer.hpp>
#include <fwWX/validator/DoubleValidator.hpp>

#include "uiVisuWx/PointEditor.hpp"
#include "fwMath/IntrasecTypes.hpp"

namespace uiVisu
{

    REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::PointEditor , ::fwData::Composite ) ;


    PointEditor::PointEditor() throw() :  m_valueX(0), m_valueY(0), m_valueZ(0)
    {
        addNewHandledEvent(::fwComEd::InteractionMsg::MOUSE_MOVE);
    }

    //------------------------------------------------------------------------------

    PointEditor::~PointEditor() throw()
    {}

    //------------------------------------------------------------------------------

    void PointEditor::starting() throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        this->::fwGui::IGuiContainerSrv::create();

        ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
        wxWindow* const container = wxContainer->getWxContainer();
        assert( container ) ;

        wxBoxSizer* hLayout = new wxBoxSizer( wxHORIZONTAL );

        wxStaticText* staticText_x = new wxStaticText( container, wxNewId(), wxT("x:"), wxDefaultPosition, wxDefaultSize, 0 );
        staticText_x->Wrap( -1 );
        hLayout->Add( staticText_x, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_textCtrl_x = new wxTextCtrl( container, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
        m_textCtrl_x->SetMinSize( wxSize( 20,-1 ) );
        m_textCtrl_x->SetValidator( ::fwWX::validator::DoubleValidator( m_valueX ) );
        hLayout->Add( m_textCtrl_x, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        wxStaticText* staticText_y = new wxStaticText( container, wxNewId(), wxT("y:"), wxDefaultPosition, wxDefaultSize, 0 );
        staticText_y->Wrap( -1 );
        hLayout->Add( staticText_y, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_textCtrl_y = new wxTextCtrl( container, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
        m_textCtrl_y->SetMinSize( wxSize( 20,-1 ) );
        m_textCtrl_y->SetValidator( ::fwWX::validator::DoubleValidator( m_valueY ) );
        hLayout->Add( m_textCtrl_y, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        wxStaticText* staticText_z = new wxStaticText( container, wxNewId(), wxT("z:"), wxDefaultPosition, wxDefaultSize, 0 );
        staticText_z->Wrap( -1 );
        hLayout->Add( staticText_z, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        m_textCtrl_z = new wxTextCtrl( container, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
        m_textCtrl_z->SetMinSize( wxSize( 20,-1 ) );
        m_textCtrl_z->SetValidator( ::fwWX::validator::DoubleValidator( m_valueZ ) );
        hLayout->Add( m_textCtrl_z, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        container->SetSizer( hLayout );
        container->Layout();

        this->updating();
    }

    //------------------------------------------------------------------------------

    void PointEditor::stopping() throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();

        this->getContainer()->clean();
        this->::fwGui::IGuiContainerSrv::destroy();
    }

    //------------------------------------------------------------------------------

    void PointEditor::configuring() throw(fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        this->::fwGui::IGuiContainerSrv::initialize();
    }

    //------------------------------------------------------------------------------

    void PointEditor::updating() throw(::fwTools::Failed)
    {
        wxString x;
        wxString y;
        wxString z;
        x << (int) m_valueX;
        y << (int) m_valueY;
        z << (int) m_valueZ;
        m_textCtrl_x->SetValue(x);
        m_textCtrl_y->SetValue(y);
        m_textCtrl_z->SetValue(z);
    }

    //------------------------------------------------------------------------------

    void PointEditor::swapping() throw(::fwTools::Failed)
    {
        this->updating();
    }

    //------------------------------------------------------------------------------

    void PointEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        ::fwComEd::InteractionMsg::csptr interactionMsg = ::fwComEd::InteractionMsg::dynamicConstCast(_msg);

        if (interactionMsg)
        {
            ::fwData::Point::csptr point = interactionMsg->getEventPoint();
            SLM_ASSERT("Sorry, the object is null", point);
            if(point)
            {
                fwVec3d  pointCoord = point->getCoord();
                ::fwData::Point::NewSptr point;
                m_valueX = pointCoord[0];
                m_valueY = pointCoord[1];
                m_valueZ = pointCoord[2];
            }
            this->updating();
        }
    }

    //------------------------------------------------------------------------------

    void PointEditor::info( std::ostream &_sstream )
    {
        _sstream << "Point Editor";
    }

    //------------------------------------------------------------------------------
}

