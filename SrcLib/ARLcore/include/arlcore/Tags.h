/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_TAGS_H__
#define __ARLCORE_TAGS_H__
#include <arlcore/Common.h>

#include <arlcore/PlaneSystem.h>
#include <fwCore/macros.hpp>
#include <arlcore/Tag.h>

namespace arlCore
{
class Object;
class Tags : public fwTools::Object
{
/**
 * @brief   Ensemble de tags de detection
 */
public:

    fwCoreClassDefinitionsWithNFactoriesMacro( (Tags)(::fwTools::Object),
                                               ((TagsFactory,((arlCore::PlaneSystem &))((const std::string &)) ))
                                               );

    ARLCORE_API static Tags::sptr TagsFactory( arlCore::PlaneSystem& universe, const std::string& name);

    //! @brief Constructor
    ARLCORE_API Tags( PlaneSystem& universe, const std::string& name = "" );

//        //! @brief Constructor by copy
//        ARLCORE_API Tags( const Tags& );

    //! @brief Affectation
    //VAG ARLCORE_API Tags& operator=( const Tags& );

    //! @brief Destructor
    ARLCORE_API ~Tags( void);

    ARLCORE_API std::string getString( void ) const;

    ARLCORE_API bool save( const std::string& fileName, bool overwrite = true ) const;

    ARLCORE_API bool load( const std::string& fileName );

    virtual ARLCORE_API SPTR(Tag) addTag( CSPTR( PointList ) pl );

    virtual ARLCORE_API bool delTag( CSPTR(Tag) );

    ARLCORE_API Tag::csptr getTag( unsigned int index ) const;

    ARLCORE_API SPTR(Tag) getTag( unsigned int index );

    ARLCORE_API unsigned int size( void ) const;

protected:
    ARLCORE_API bool reset( void );
    ARLCORE_API bool copy(  CSPTR(Tags)  );
    CSPTR(Tag) addTag( SPTR(Tag) );

    PlaneSystem& m_universe;
    std::vector< SPTR(Tag) > m_tags;
};
} // namespace arlCore
#endif // __ARLCORE_TAGS_H__
