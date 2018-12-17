/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWRENDERQT_BSPLINE_HPP__
#define __FWRENDERQT_BSPLINE_HPP__

#include <cmath>
#include <iostream>
#include <list>
#include <vector>

#define DEFAULT_PRECISION 40

struct point
{
    float x;
    float y;

    point(float _x = 0.f, float _y = 0.f) :
        x(_x),
        y(_y)
    {
    }
    point(const point& _which)
    {
        this->x = _which.x;
        this->y = _which.y;
    }
    ~point()
    {
    }

    //------------------------------------------------------------------------------

    point& operator = (const point& _which)
    {
        this->x = _which.x;
        this->y = _which.y;
        return *this;
    }
};

class point_list
{
public:
    point_list()
    {
    }
    point_list(const point_list& _which)
    {
        this->m_list = _which.m_list;
    }
    ~point_list()
    {
        for (unsigned int i = 0; i < m_list.size(); i++)
        {
            delete m_list[i];
        }
    }

    //------------------------------------------------------------------------------

    point& get_point(int which)
    {
        point* pt = m_list.at(which);
        return *pt;
    }

    //------------------------------------------------------------------------------

    void add_point(point* _which)
    {
        m_list.push_back(_which);
    }

    std::vector<point*> m_list;
};

class cat_curve
{
public:
    cat_curve(const point_list& _which) :
        m_plist(new point_list(_which)),
        m_curve_point(nullptr),
        m_nb_curve_point(0),
        m_nb_point(m_plist->m_list.size()),
        m_precision(DEFAULT_PRECISION),
        m_draw_number(true)
    {
    }

    ~cat_curve()
    {
        if (m_curve_point != nullptr)
        {
            delete[] m_curve_point;
        }
    }

    //------------------------------------------------------------------------------

    void compute()
    {
        if (m_curve_point != nullptr)
        {
            delete[] m_curve_point;
        }
        m_curve_point    = new point[m_precision+1];
        m_nb_curve_point = m_precision+1;

        float t = 1.0f, p = (float)(m_nb_point-3)/(float)m_precision;

        int i;
        float ti;
        for(int k = 0; k < m_nb_curve_point; k++)
        {
            i  = (int)floor(t);
            ti = (float)i;
            if (i < m_nb_point-2)
            {
                m_curve_point[k].x =
                    (3.0f*m_plist->m_list[i]->x-3.0f*m_plist->m_list[i+1]->x+
                     m_plist->m_list[i+2]->x-m_plist->m_list[i-1]->x)*pow(t-ti, 3)/2.0f+
                    ( 2.0f*m_plist->m_list[i-1]->x - 5.0f*m_plist->m_list[i]->x +
                      4.0f*m_plist->m_list[i+1]->x - m_plist->m_list[i+2]->x)*pow(t-ti, 2)/2.0f+
                    ( m_plist->m_list[i+1]->x - m_plist->m_list[i-1]->x ) *(t-ti)/2.0f + m_plist->m_list[i]->x;

                m_curve_point[k].y =
                    (3.0f*m_plist->m_list[i]->y-3.0f*m_plist->m_list[i+1]->y+
                     m_plist->m_list[i+2]->y-m_plist->m_list[i-1]->y)*pow(t-ti, 3)/2.0f+
                    ( 2.0f*m_plist->m_list[i-1]->y - 5.0f*m_plist->m_list[i]->y +
                      4.0f*m_plist->m_list[i+1]->y - m_plist->m_list[i+2]->y)*pow(t-ti, 2)/2.0f+
                    ( m_plist->m_list[i+1]->y - m_plist->m_list[i-1]->y ) *(t-ti)/2.0f + m_plist->m_list[i]->y;
            }
            else if(i < m_nb_point)
            {
                m_curve_point[k].x = m_plist->m_list[i]->x;
                m_curve_point[k].y = m_plist->m_list[i]->y;
            }

            t += p;
        }
    }

    point* m_curve_point;
    int m_nb_curve_point;

    point_list* m_plist;
    int m_nb_point;

    int m_precision;
    bool m_draw_number;
};

#endif //__FWRENDERQT_BSPLINE_HPP__

