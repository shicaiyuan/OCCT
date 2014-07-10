// Created on: 2014-06-03
// Created by: Oleg AGASHIN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_WireChecker_HeaderFile
#define _BRepMesh_WireChecker_HeaderFile

#include <Standard.hxx>
#include <TopoDS_Face.hxx>
#include <BRepMesh_Status.hxx>
#include <Handle_BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_ClassifierPtr.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_EBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Edge.hxx>
#include <Bnd_Box2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>

#include <vector>

class BRepMesh_DataMapOfShapePairOfPolygon;
class TColStd_IndexedMapOfInteger;
class BRepMesh_DataStructureOfDelaun;

//! Auxilary class intended to check correctness of discretized face.
//! In particular, checks boundaries of discretized face for self 
//! intersections and gaps.
class BRepMesh_WireChecker
{
public:

  //! Structure keeping parameters of segment.
  struct Segment
  {
    gp_XY StartPnt;
    gp_XY EndPnt;
  };

  typedef NCollection_EBTree<Standard_Integer, Bnd_Box2d>       BndBox2dTree;
  typedef NCollection_UBTreeFiller<Standard_Integer, Bnd_Box2d> BndBox2dTreeFiller;
  typedef NCollection_Array1<Standard_Integer>                  ArrayOfInteger;
  typedef std::vector<Segment>                                  ArrayOfSegments;
  typedef NCollection_Handle<ArrayOfSegments>                   HArrayOfSegments;
  typedef NCollection_Handle<BndBox2dTree>                      HBndBox2dTree;
  typedef std::pair<HArrayOfSegments, HBndBox2dTree>            SegmentsTree;

  //! Selector.
  //! Used to identify segments with overlapped bounding boxes.
  //! Note that instance of selector can be used only once due to
  //! unextentable array of indices.
  class BndBox2dTreeSelector : public BndBox2dTree::Selector
  {
    public:
      Standard_EXPORT BndBox2dTreeSelector(const Standard_Integer theReservedSize);
      Standard_EXPORT virtual Standard_Boolean Reject(const Bnd_Box2d& theBox2D) const;
      Standard_EXPORT virtual Standard_Boolean Accept(const Standard_Integer& theIndex);
      
      Standard_EXPORT void Clear();
      Standard_EXPORT void SetBox(const Bnd_Box2d& theBox2D);
      Standard_EXPORT void SetSkippedIndex(const Standard_Integer theIndex);
      Standard_EXPORT const ArrayOfInteger& Indices() const;
      Standard_EXPORT Standard_Integer IndicesNb() const;

    protected:
      Bnd_Box2d        myBox2D;
      Standard_Integer mySkippedIndex;
      ArrayOfInteger   myIndices;
      Standard_Integer myIndicesNb;
  };

private:

  typedef NCollection_List<TopoDS_Edge>                           ListOfEdges;
  typedef std::vector<ListOfEdges>                                SeqOfWireEdges;
  typedef NCollection_Sequence<gp_Pnt2d>                          SeqOfPnt2d;
  typedef std::vector<SeqOfPnt2d>                                 SeqOfDWires;
  typedef NCollection_DataMap<Standard_Integer, Standard_Integer> DataMapIntInt;
  typedef std::pair<Standard_Integer, gp_XY>                      PairIntPnt;

public:

  //! Constructor.
  //! \param theFace Face to be checked.
  //! \param theTolUV Tolerance to be used for calculations in parametric space.
  //! \param theEdges Map of edges with associated polygon on triangulation.
  //! \param theVertexMap Map of face vertices.
  //! \param theStructure Discretized representation of face in parametric space.
  //! \param theUmin Lower U boundary of the face in parametric space.
  //! \param theUmax Upper U boundary of the face in parametric space.
  //! \param theVmin Lower V boundary of the face in parametric space.
  //! \param theVmax Upper V boundary of the face in parametric space.
  Standard_EXPORT BRepMesh_WireChecker(
    const TopoDS_Face&                            theFace,
    const Standard_Real                           theTolUV,
    const BRepMesh_DataMapOfShapePairOfPolygon&   theEdges,
    const TColStd_IndexedMapOfInteger&            theVertexMap,
    const Handle(BRepMesh_DataStructureOfDelaun)& theStructure,
    const Standard_Real                           theUmin,
    const Standard_Real                           theUmax,
    const Standard_Real                           theVmin,
    const Standard_Real                           theVmax,
    const Standard_Boolean                        isInParallel);

  //! Recompute data using parameters passed in constructor.
  //! \param[out] theClassifier Classifier to be updated using calculated data.
  Standard_EXPORT void ReCompute(BRepMesh_ClassifierPtr& theClassifier);

  //! Returns status of the check.
  inline BRepMesh_Status Status() const
  {
    return myStatus;
  }

private:

  //! Collects dis�rete wires.
  //! \param[out] theDWires sequence of discretized wires to be filled.
  //! \return TRUE on success, FALSE in case of open wire.
  Standard_Boolean collectDiscretizedWires(SeqOfDWires& theDWires);

  //! Fills array of BiPoints for corresponding wire.
  //! \param theDWires Sequence of wires to be processed.
  //! \param theWiresSegmentsTree Array of segments with corresponding 
  //! bounding boxes trees to be filled.
  void fillSegmentsTree(
    const SeqOfDWires&         theDWires, 
    std::vector<SegmentsTree>& theWiresSegmentsTree);

  //! Assignment operator.
  void operator =(BRepMesh_WireChecker& /*theOther*/)
  {
  }

private:

  const Standard_Real                           myTolUV;
  const BRepMesh_DataMapOfShapePairOfPolygon&   myEdges;
  const TColStd_IndexedMapOfInteger&            myVertexMap;
  const Handle(BRepMesh_DataStructureOfDelaun)& myStructure;
  const Standard_Real                           myUmin;
  const Standard_Real                           myUmax;
  const Standard_Real                           myVmin;
  const Standard_Real                           myVmax;
  BRepMesh_Status                               myStatus;
  SeqOfWireEdges                                myWiresEdges;
  Standard_Boolean                              myIsInParallel;
};

#endif
