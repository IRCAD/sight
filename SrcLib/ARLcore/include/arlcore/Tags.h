/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_TAGS_H
#define _ARLCORE_TAGS_H
#include <arlcore/Common.h>

#include <arlcore/PlaneSystem.h>
#include <arlcore/Tag.h>

namespace arlCore
{
    class Object;
    class Tags : public Object
    {
    /**
     * @class   Tags
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Ensemble de tags de detection
     */
    public:
        //! @brief Constructor
        ARLCORE_API Tags( PlaneSystem &universe, const std::string &name="" );

        //! @brief Constructor by copy
        ARLCORE_API Tags( const Tags& );

        //! @brief Affectation
        ARLCORE_API Tags& operator=( const Tags& );

        //! @brief Destructor
        ARLCORE_API ~Tags( void);

        ARLCORE_API std::string getString( void ) const;

        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        ARLCORE_API bool load( const std::string &fileName );

        virtual ARLCORE_API Tag* addTag( const PointList &pl );

        virtual ARLCORE_API bool delTag( const Tag* );

        ARLCORE_API const Tag* getTag( unsigned int index ) const;

        ARLCORE_API Tag* getTag( unsigned int index );

        ARLCORE_API unsigned int size( void ) const;

    protected:
        ARLCORE_API bool reset( void );
        ARLCORE_API bool copy(  const Tags& t );
        const Tag* addTag( Tag* );

        PlaneSystem &m_universe;
        std::vector< Tag* > m_tags;
    };
} // namespace arlCore
#endif // _ARLCORE_TAGS_H
