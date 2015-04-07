// Created on: 2015-03-16
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _SelectMgr_Frustum_HeaderFile
#define _SelectMgr_Frustum_HeaderFile

#include <BVH_Box.hxx>
#include <gp_Pnt.hxx>
#include <SelectMgr_BaseFrustum.hxx>
#include <SelectMgr_FrustumBuilder.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//! This is an internal class containing representation of rectangular selecting frustum, created in case
//! of point and box selection, and algorithms for overlap detection between selecting
//! frustum and sensitive entities. The principle of frustum calculation:
//! - for point selection: on a near view frustum plane rectangular neighborhood of
//!                        user-picked point is created according to the pixel tolerance
//!                        given and then this rectangle is projected onto far view frustum
//!                        plane. This rectangles define the parallel bases of selecting frustum;
//! - for box selection: box points are projected onto near and far view frustum planes.
//!                      These 2 projected rectangles define parallel bases of selecting frustum.
//! Overlap detection tests are implemented according to the terms of separating axis
//! theorem (SAT).
//! Vertex order:
//! - for triangular frustum: V0_Near, V1_Near, V2_Near,
//!                           V0_Far, V1_Far, V2_Far;
//! - for rectangular frustum: LeftTopNear, LeftTopFar,
//!                            LeftBottomNear,LeftBottomFar,
//!                            RightTopNear, RightTopFar,
//!                            RightBottomNear, RightBottomFar.
//! Plane order in array:
//! - for triangular frustum: V0V1, V1V2, V0V2, Near, Far;
//! - for rectangular frustum: Top, Bottom, Left, Right, Near, Far.
//! Uncollinear edge directions order:
//! - for rectangular frustum: Horizontal, Vertical,
//!                            LeftLower, RightLower,
//!                            LeftUpper, RightUpper;
//! - for triangular frustum: V0_Near - V0_Far, V1_Near - V1_Far, V2_Near - V2_Far,
//!                           V1_Near - V0_Near, V2_Near - V1_Near, V2_Near - V0_Near.
template <int N>
class SelectMgr_Frustum : public SelectMgr_BaseFrustum
{
public:

  SelectMgr_Frustum() : SelectMgr_BaseFrustum() {};

protected:

  // SAT Tests for different objects

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  const Standard_Boolean hasOverlap (const SelectMgr_Vec3& theMinPnt,
                                     const SelectMgr_Vec3& theMaxPnt);

  //! SAT intersection test between defined volume and given point
  const Standard_Boolean hasOverlap (const gp_Pnt& thePnt);

  //! SAT intersection test between defined volume and given segment
  const Standard_Boolean hasOverlap (const gp_Pnt& thePnt1,
                                     const gp_Pnt& thePnt2);

  //! SAT intersection test between frustum given and planar convex polygon represented as ordered point set
  const Standard_Boolean hasOverlap (const Handle(TColgp_HArray1OfPnt)& theArrayOfPnts,
                                     SelectMgr_Vec3& theNormal);

  //! SAT intersection test between defined volume and given triangle
  const Standard_Boolean hasOverlap (const gp_Pnt& thePnt1,
                                     const gp_Pnt& thePnt2,
                                     const gp_Pnt& thePnt3,
                                     SelectMgr_Vec3& theNormal);

private:

  //! Checks if AABB and frustum are separated along the given axis
  Standard_Boolean isSeparated (const SelectMgr_Vec3& theBoxMin,
                                const SelectMgr_Vec3& theBoxMax,
                                const SelectMgr_Vec3& theAxis) const;

  //! Checks if triangle and frustum are separated along the given axis
  Standard_Boolean isSeparated (const gp_Pnt& thePnt1,
                                const gp_Pnt& thePnt2,
                                const gp_Pnt& thePnt3,
                                const SelectMgr_Vec3& theAxis) const;

protected:

  SelectMgr_Vec3 myPlanes[N + 2];        //!< Plane equations
  SelectMgr_Vec3 myVertices[N * 2];      //!< Vertices coordinates

  Standard_Real myMaxVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMinVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMaxOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors
  Standard_Real myMinOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors

  SelectMgr_Vec3 myEdgeDirs[6];                    //!< Cached edge directions
};

#include <SelectMgr_Frustum.lxx>

#endif // _SelectMgr_Frustum_HeaderFile