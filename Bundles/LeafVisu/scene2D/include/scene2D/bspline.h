/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _BSPLINE_H
#define _BSPLINE_H

#include <iostream>
#include <cmath>
#include <list>
#include <vector>

//#include "SegmentationPeritoineSpline3D/bspline.hpp"
#define BUFFER_SIZE 128
#define DEFAULT_PRECISION 40

using namespace std;

struct point
{
    float x;
    float y;

    //float r,g,b;
    point(float _x = 0, float _y = 0): x(_x), y(_y) { }
    point(const point & _which)
    {
        this->x = _which.x;
        this->y = _which.y;
    }
    ~point() {}

    point & operator = (const point & _which)
    {
        this->x = _which.x;
        this->y = _which.y;
        return *this;
    }
};
class point_list
{
public:
    point_list() {}
    point_list(const point_list & _which)
    {
        this->list = _which.list;
    }
    ~point_list()
    {
        for (unsigned int i = 0; i<this->list.size(); i++) delete list[i];
    }

    point & get_point(int which)
    {
        std::vector<point*>::iterator _it = this->list.begin();
        for (int i = 0; i < which; i++) _it++;
        return **_it;
    }

    void add_point(point * _which)
    {
        this->list.push_back(_which);
    }

    std::vector<point*> list;
};


class cat_curve
{
public:
    cat_curve(const point_list & _which)
    {
        this->plist = new point_list(_which);
        this->curve_point = NULL;
        this->nb_curve_point = 0;
        this->nb_point = this->plist->list.size();
        this->precision = DEFAULT_PRECISION;
        this->draw_number = true;
    }
    ~cat_curve()
    {
        if (this->curve_point != NULL) delete[] this->curve_point;
    }

    void compute()
    {
        if (this->curve_point != NULL) delete[] this->curve_point;
        this->curve_point = new point[this->precision+1];
        this->nb_curve_point = this->precision+1;

        float t=1.0f, p = (float)(this->nb_point-3)/(float)this->precision;

        int i;
        float ti;
        for(int k=0; k<this->nb_curve_point; k++)
        {
            i = (int)floor(t);
            ti = (float)i;
            if (i!= this->nb_point-2)
            {
                this->curve_point[k].x = 
                    (3.0f*this->plist->list[i]->x-3.0f*this->plist->list[i+1]->x+
                    this->plist->list[i+2]->x-this->plist->list[i-1]->x)*pow(t-ti,3)/2.0f+
                    ( 2.0f*this->plist->list[i-1]->x - 5.0f*this->plist->list[i]->x + 
                    4.0f*this->plist->list[i+1]->x - this->plist->list[i+2]->x)*pow(t-ti,2)/2.0f+ 
                    ( this->plist->list[i+1]->x - this->plist->list[i-1]->x ) *(t-ti)/2.0f + this->plist->list[i]->x;

                this->curve_point[k].y = 
                    (3.0f*this->plist->list[i]->y-3.0f*this->plist->list[i+1]->y+
                    this->plist->list[i+2]->y-this->plist->list[i-1]->y)*pow(t-ti,3)/2.0f+
                    ( 2.0f*this->plist->list[i-1]->y - 5.0f*this->plist->list[i]->y + 
                    4.0f*this->plist->list[i+1]->y - this->plist->list[i+2]->y)*pow(t-ti,2)/2.0f+
                    ( this->plist->list[i+1]->y - this->plist->list[i-1]->y ) *(t-ti)/2.0f + this->plist->list[i]->y;
            }
            else
            {
                this->curve_point[k].x = this->plist->list[i]->x;
                this->curve_point[k].y = this->plist->list[i]->y;
            }

            t += p;
        }
    }

    point * curve_point;
    int nb_curve_point;

    point_list * plist;
    int nb_point;

    int precision;
    bool draw_number;
};
#endif 

