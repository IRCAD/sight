/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_SLICESELECTOR_HPP_
#define _FWWX_SLICESELECTOR_HPP_

#include <boost/function.hpp>

#include <wx/event.h>

#include "fwWX/config.hpp"


// wxWidgets
class wxSlider;
class wxWindow;
class wxTextCtrl;
class wxPanel;

namespace fwWX
{

/**
 * @brief A wxWidgets panel used to control a VTK 2D Negatoscope view.
 * @author IRCAD (Research and Development Team).
 */
class SliceSelector : public wxPanel
{
public:
    /**
     * @brief Constructor.
     * @param parent The parent window.
     * @param id     The window ID.
     */
        FWWX_API SliceSelector( wxWindow* const parent, const wxWindowID id = wxID_ANY ) throw();


    /// @brief Destructor.
        FWWX_API virtual ~SliceSelector() throw();

        FWWX_API void setSliceRange( int min, int max );

        FWWX_API void setSliceValue( int index );

        FWWX_API void setTypeSelection( int type );

        FWWX_API void setEnable(bool enable);

        typedef ::boost::function1<void, int > ChangeIndexCallback;
        FWWX_API void setChangeIndexCallback(ChangeIndexCallback fct);

        typedef ::boost::function1<void, int > ChangeTypeCallback;
        FWWX_API void setChangeTypeCallback(ChangeTypeCallback fct);

protected:

#ifdef __MACOSX__
        FWWX_API void onSliceDownButton( wxCommandEvent& event );
        FWWX_API void onSliceTypeDownButton( wxCommandEvent& event );
#else
        /**
         * @brief Event handler for a slice type change.
         * @param event The corresponding wxWidgets event.
         */
        FWWX_API void onSliceTypeChange( wxCommandEvent& event );
#endif

        /**
         * @brief Event handler for a slice index change.
         * @param event The corresponding wxWidgets event.
         */
        FWWX_API void onSliceIndexChange( wxScrollEvent& event ) throw();

    /// @brief wxWidgets event table.
    /// @{
    DECLARE_EVENT_TABLE()
    /// @}

private:

    /// @brief The IDs for the wxWidgets controls.
    enum {
        ID_SLICE_INDEX = wxID_HIGHEST + 1,
        ID_SLICE_TEXT,
        ID_SLICE_TYPE,
        ID_SAGITTAL_BTN,
        ID_FRONTAL_BTN,
        ID_AXIAL_BTN
    };


#ifdef __MACOSX__
    wxButton* m_sliceType;
    wxMenu*   m_sliceMenu;
    int m_sliceTypeSelection;
    wxMenuItem *m_pItemAxial;
    wxMenuItem *m_pItemFrontal;
    wxMenuItem *m_pItemSagittal;
#else
    wxChoice* m_sliceType;
#endif

    /// @brief Slice type names as a string array for the choice widget.
    static const wxString m_sliceTypes[];

    /// @brief Slice type names as a wxWidgets string array.
    static const wxArrayString m_sliceTypesArray;

    /// @brief The slice index slider widget.
    wxSlider*   m_sliceIndex;
    wxTextCtrl * m_pSliceIndexText;

    void printIndex(int index);
    void printType(int type);

    ChangeIndexCallback m_fctChangeIndexCallback;
    ChangeTypeCallback m_fctChangeTypeCallback;
};

} // fwWX

#endif // _FWWX_SLICESELECTOR_HPP_
