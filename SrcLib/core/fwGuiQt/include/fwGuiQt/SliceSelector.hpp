/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_SLICESELECTOR_HPP_
#define _FWGUIQT_SLICESELECTOR_HPP_


#include <QObject>
#include <QPointer>
#include <QWidget>

#include <boost/function.hpp>

#include "fwGuiQt/config.hpp"


// Qt
class QSlider;
class QLineEdit;
class QComboBox;
class QStringList;

namespace fwGuiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 * @author IRCAD (Research and Development Team).
 */
class SliceSelector : public QWidget
{

    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent window.
     * @param id     The window ID.
     */
    FWGUIQT_API SliceSelector( QWidget* const parent) throw();


    /// @brief Destructor.
    FWGUIQT_API virtual ~SliceSelector() throw();

    FWGUIQT_API void setSliceRange( int min, int max );

    FWGUIQT_API void setSliceValue( int index );

    FWGUIQT_API void setTypeSelection( int type );

    FWGUIQT_API void setEnable(bool enable);

    typedef ::boost::function1<void, int > ChangeIndexCallback;
    FWGUIQT_API void setChangeIndexCallback(ChangeIndexCallback fct);

    typedef ::boost::function1<void, int > ChangeTypeCallback;
    FWGUIQT_API void setChangeTypeCallback(ChangeTypeCallback fct);

protected slots:

    /**
     * @brief Event handler for a slice type change.
     * @param event The corresponding wxWidgets event.
     */
    FWGUIQT_API void onSliceTypeChange( int index );

    /**
     * @brief Event handler for a slice index change.
     * @param event The corresponding wxWidgets event.
     */
    FWGUIQT_API void onSliceIndexChange( int value ) throw();

private:

    QPointer< QComboBox > m_sliceType;

    /// @brief Slice type names as a qt string array.
    static const QStringList m_sliceTypesArray;

    /// @brief The slice index slider widget.
    QPointer< QSlider > m_sliceIndex;
    QPointer< QLineEdit > m_pSliceIndexText;

    void printIndex(int index);
    void printType(int type);

    ChangeIndexCallback m_fctChangeIndexCallback;
    ChangeTypeCallback m_fctChangeTypeCallback;
};

} // fwGuiQt

#endif // _FWGUIQT_SLICESELECTOR_HPP_
