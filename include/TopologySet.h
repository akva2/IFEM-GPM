#ifndef _TOPOLOGYSET_H
#define _TOPOLOGYSET_H

#include <set>
#include <vector>
#include <GoTools/trivariate/SplineVolume.h>

class Volume;
class Face;
class Line;
class Vertex;

/**********************************************************************************//**
 * \brief Main class for keeping track of the module topology
 * 
 * This class keeps control of all topological relations needed to create a consistent
 * local-to-global enumeration mapping. It relies on comparison of control points for 
 * its construction, so it is recommended that one creates the topology first, and uses
 * this in all subsequent computations (i.e. before refinement). 
 *************************************************************************************/
class TopologySet {
	public:
		TopologySet(std::vector<boost::shared_ptr<Go::SplineVolume> > &spline_volumes, double tol=1e-4);
		void buildTopology();
		int numbVertices() const;
		int numbLines() const;
		int numbFaces() const;
		int numbVolumes() const;
		
		std::set<Vertex*>::iterator vertex_begin();
		std::set<Vertex*>::iterator vertex_end();
		std::set<Line*>::iterator line_begin();
		std::set<Line*>::iterator line_end();
		std::set<Face*>::iterator face_begin();
		std::set<Face*>::iterator face_end();

	private:
		void    addVolume(Volume* v);
		Face*   addFace(Face* f);
		Line*   addLine(Line* l);
		Vertex* addVertex(Vertex* v);

		double tol;                  //!< Control point tolerance (given in euclidean distance)
		std::set<Volume*> volume_;   //!< All unique volumes
		std::set<Face*>   face_;     //!< All unique faces
		std::set<Line*>   line_;     //!< All unique edge lines
		std::set<Vertex*> vertex_;   //!< All unique corner vertices
		std::vector<boost::shared_ptr<Go::SplineVolume> > spline_volumes_; //!< Spline objects

};

#endif
