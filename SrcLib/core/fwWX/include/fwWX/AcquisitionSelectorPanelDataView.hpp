/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_ACQUISITIONSELECTORPANELDATAVIEW_HPP_
#define _FWWX_ACQUISITIONSELECTORPANELDATAVIEW_HPP_

#include <vector>
#include <map>
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/datetime.h>
#include <boost/shared_ptr.hpp>

#include "fwWX/config.hpp"
#include "fwWX/PatientDBTreeModel.hpp"

namespace fwData
{
    class PatientDB;
}

namespace fwWX
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   AcquisitionSelectorPanelDataView.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @todo AcquisitionSelectorPanelDataView is not commented.
 */
class FWWX_CLASS_API AcquisitionSelectorPanelDataView : public wxPanel
{


public :

    enum
    {
        ID_LIST = wxID_HIGHEST + 1,
        ID_PANELDATAVIEW,
        ID_MENU_PATIENT_DELETE,
        ID_PROPGRID
    };

    FWWX_API AcquisitionSelectorPanelDataView( wxWindow * _container, ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, ::fwServices::IService::wptr _serviceParent );

    FWWX_API ~AcquisitionSelectorPanelDataView();

    void FWWX_API onItemSelected( wxDataViewEvent & event );

    void FWWX_API itemSelectionNotification( /*long itemIndex*/ );

    void FWWX_API updateData( ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, bool _bResetSelection = true );

    const FWWX_API std::vector<int> getIndexOfSelectedItem();

    void UpdateProperty(wxPropertyGridEvent& event);

private :

    void SelectLastItem();
    void SelectItem( std::vector<int> select);
    void SelectItem( fwDataNode* select);

    void updatePropGrid();

    std::vector< int > m_vSelectedItem;
    wxDataViewCtrl * m_wxDataViewCtrl;
    ::PatientDBTreeModel* m_patientDbTree_model;

    ::boost::weak_ptr< ::fwData::PatientDB > m_associatedPatientDB;


    wxPropertyGrid *m_pg;
    std::vector< std::pair < ::fwData::Object::sptr, std::vector<const wxPGProperty *> > > m_vObjectEditor;
    std::pair < ::fwData::Object::wptr, const wxPGProperty * >  m_showAllRecPGProperty;

    ::fwServices::IService::wptr m_serviceParent;

    DECLARE_EVENT_TABLE()
};

} // namespace fwWX

#endif /* _FWWX_ACQUISITIONSELECTORPANELDATAVIEW_HPP_ */

