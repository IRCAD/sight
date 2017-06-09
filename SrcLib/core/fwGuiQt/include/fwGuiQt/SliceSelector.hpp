/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_SLICESELECTOR_HPP__
#define __FWGUIQT_SLICESELECTOR_HPP__

#include "fwGuiQt/config.hpp"

#include <boost/function.hpp>

#include <QObject>
#include <QPointer>
#include <QWidget>

// Qt
class QSlider;
class QLineEdit;
class QComboBox;
class QStringList;

namespace fwGuiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 *
 */
class SliceSelector : public QWidget
{

Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent widget.
     */
    FWGUIQT_API SliceSelector( QWidget* const parent = nullptr) throw();

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

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param index index of the selected type item.
     */
    FWGUIQT_API void onSliceTypeChange( int index );

    /**
     * @brief Event handler for a slice index change.
     * @param value current value of the slice index slider.
     */
    FWGUIQT_API void onSliceIndexChange( int value ) throw();

private:

    QPointer< QComboBox > m_sliceType;

    /// @brief The slice index slider widget.
    QPointer< QSlider > m_sliceIndex;
    QPointer< QLineEdit > m_pSliceIndexText;

    void printIndex(int index);
    void printType(int type);

    ChangeIndexCallback m_fctChangeIndexCallback;
    ChangeTypeCallback m_fctChangeTypeCallback;
};

} // fwGuiQt

#endif // __FWGUIQT_SLICESELECTOR_HPP__
