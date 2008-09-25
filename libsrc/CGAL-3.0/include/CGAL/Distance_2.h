// Copyright (c) 1997  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source: /CVSROOT/CGAL/Packages/Triangulation_2/include/CGAL/Distance_2.h,v $
// $Revision: 1.6 $ $Date: 2003/09/18 10:26:05 $
// $Name: current_submission $
//
// Author(s)     : Mariette Yvinec
//                 Sylvain Pion

#ifndef CGAL_DISTANCE_2_H
#define CGAL_DISTANCE_2_H

#include <CGAL/assertions.h>
#include <CGAL/distance_predicates_2.h>

CGAL_BEGIN_NAMESPACE

template <class I>
struct Distance_2
{
  typedef typename I::Point Point;

  Distance_2(const I* = NULL) {}

  Distance_2(const Point& p0, const I* = NULL)
  { p[0]=p0; }

  Distance_2(const Point& p0, const Point& p1, const I* = NULL)
  { p[0]=p0; p[1]=p1; }

  Distance_2(const Point& p0, const Point& p1, const Point& p2,
	     const I* = NULL)
  { p[0]=p0; p[1]=p1; p[2]=p2; }

  void set_point(int i, const Point& q)
  {
    CGAL_precondition( ((unsigned int) i) < 3 );
    p[i] = q;
  }

  Point get_point(int i) const
  {
    CGAL_precondition( ((unsigned int) i) < 3 );
    return p[i];
  }

  Comparison_result compare() const
  {
    return cmp_dist_to_point(p[0], p[1], p[2]);
  }

private:
  Point p[3];
};

template <class Traits>
struct Distance_xy_3  : public Distance_2<Traits>
{
  typedef typename Distance_2<Traits>::Point Point;

public:
  Distance_xy_3(const Point& p0,
	   const Traits* traits = NULL)
    : Distance_2<Traits>(p0, traits) { }


  Distance_xy_3(const Point& p0,
	   const Point& p1,
	   const Traits* traits = NULL)
    : Distance_2<Traits>(p0,p1,traits) { }

  Distance_xy_3(const Point& p0,
	   const Point& p1,
	   const Point& p2,
	   const Traits* traits = NULL)
    : Distance_2<Traits>(p0,p1,p2,traits) { }

  Comparison_result
  compare() const
    {
      Point p0 = get_point(0);
      Point p1 = get_point(1);
      Point p2 = get_point(2);
      return cmp_dist_to_pointC2(x(p0),y(p0),x(p1),y(p1),x(p2),y(p2));
    }
};

CGAL_END_NAMESPACE

#endif // CGAL_DISTANCE_2_H
