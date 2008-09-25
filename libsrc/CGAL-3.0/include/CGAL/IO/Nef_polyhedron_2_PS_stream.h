// Copyright (c) 1997-2000  Max-Planck-Institute Saarbrucken (Germany).
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
// $Source: /CVSROOT/CGAL/Packages/Nef_2/include/CGAL/IO/Nef_polyhedron_2_PS_stream.h,v $
// $Revision: 1.3 $ $Date: 2003/09/18 10:23:25 $
// $Name: current_submission $
//
// Author(s)     : Michael Seel <seel@mpi-sb.mpg.de>

#ifndef NEF_POLYHEDRON_2_PS_STREAM_H
#define NEF_POLYHEDRON_2_PS_STREAM_H

#include <CGAL/Nef_polyhedron_2.h>
#include <LEDA/gen_polygon.h>
#include <LEDA/ps_file.h>
#include <LEDA/color.h>

CGAL_BEGIN_NAMESPACE
static int frame_default = 100;

template <typename CIRC>
leda_list<leda_point>
face_cycle(CIRC c)
{
  leda_list<leda_point> L;
  CIRC c_end(c);
  CGAL_For_all(c,c_end) {
    L.append(leda_point(CGAL::to_double(c->vertex()->point().x()),
                        CGAL::to_double(c->vertex()->point().y())));
  }
  return L;
}


template <typename T>
ps_file& operator<<(ps_file& PS, const Nef_polyhedron_2<T>& P)
{
  typedef Nef_polyhedron_2<T> NefPoly;
  typedef typename NefPoly::Const_decorator  PMCDec;
  typedef typename NefPoly::Point            Point;
  typedef typename NefPoly::Line             Line;
  typedef typename T::RT  RT;
  typedef typename T::Standard_RT Standard_RT;

  PMCDec D = P.explorer();
  const T& E = Nef_polyhedron_2<T>::EK;

  Standard_RT frame_radius = frame_default;
  E.determine_frame_radius(D.points_begin(),D.points_end(),frame_radius);
  RT::set_R(frame_radius);
  double r = CGAL::to_double(frame_radius);
  PS.init(-r,r,-r);

  typedef typename PMCDec::Vertex_const_iterator   Vertex_const_iterator;
  typedef typename PMCDec::Halfedge_const_iterator Halfedge_const_iterator;
  typedef typename PMCDec::Face_const_iterator     Face_const_iterator;
  typedef typename PMCDec::Hole_const_iterator     Hole_const_iterator;
  typedef typename PMCDec::Halfedge_around_face_const_circulator
    Halfedge_around_face_const_circulator;

  Face_const_iterator fit = D.faces_begin();
  for (++fit; fit != D.faces_end(); ++fit) {
    leda_list<leda_polygon> LP;
    Halfedge_around_face_const_circulator hfc(fit->halfedge());
    LP.append(leda_polygon(face_cycle(hfc),leda_polygon::NO_CHECK));
    Hole_const_iterator hit;
    for (hit = fit->fc_begin(); hit != fit->fc_end(); ++hit) {
      hfc = Halfedge_around_face_const_circulator(hit);
      LP.append(leda_polygon(face_cycle(hfc),leda_polygon::NO_CHECK));
    }
    leda_gen_polygon GP(LP,leda_gen_polygon::NO_CHECK);
    if ( fit->mark() ) PS.set_fill_color(leda_grey3);
    else               PS.set_fill_color(leda_white);
    PS << GP;
  }

  Halfedge_const_iterator hit;
  for (hit = D.halfedges_begin(); hit != D.halfedges_end(); ++(++hit)) {
    leda_segment s(CGAL::to_double(hit->vertex()->point().x()),
		   CGAL::to_double(hit->vertex()->point().y()),
		   CGAL::to_double(hit->opposite()->vertex()->point().x()),
		   CGAL::to_double(hit->opposite()->vertex()->point().y()));
    if ( hit->mark() ) PS.set_color(leda_black);
    else               PS.set_color(leda_grey1);
    PS << s;
  }

  PS.set_node_width(3);
  Vertex_const_iterator v;
  for (v = D.vertices_begin(); v != D.vertices_end(); ++v) {
    leda_point p(CGAL::to_double(v->point().x()),
		 CGAL::to_double(v->point().y()));
    leda_color pc;
    if ( v->mark() ) pc = leda_black;
    else             pc = leda_grey1;
    PS.draw_filled_node(p,pc);
  }


  return PS;
}

CGAL_END_NAMESPACE

#endif // NEF_POLYHEDRON_2_PS_STREAM_H
