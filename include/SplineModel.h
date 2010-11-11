#ifndef _SPLINE_MODEL_H
#define _SPLINE_MODEL_H

#include <vector>
#include <GoTools/trivariate/SplineVolume.h>

class TopologySet;

/**********************************************************************************//**
 * \brief Global number ordering (gno) structure for surface (2d) models
 *
 * Contains all necessary information to go from any local enumeration (i,j) to a global enumeration.
 * One surfGlobNumber-struct should be available for each topological face
 *************************************************************************************/
typedef struct surfGlobNumber {
	int vertex[4];      //!< global number of the 4 corner vertices
	int edge[4];        //!< start number for the 4 edge lines
	int edge_incr[4];   //!< +1 or -1 depending on whether the numbers are ascending or descending
	int surface;        //!< start number for the internal nodes on the surface
} surfGlobNumber;

/**********************************************************************************//**
 * \brief Global number ordering (gno) structure for volume (3d) models
 *
 * Contains all necessary information to go from any local enumeration (i,j,k) to a global enumeration.
 * One volGlobNumber-struct should be available for each topological volume
 *************************************************************************************/
typedef struct volGlobNumber {
	int vertex[8];          //!< global number of the 8 corner vertices
	int edge[12];           //!< start number for the 12 edge lines
	int edge_incr[12];      //!< +1 or -1 depending on whether the numbers are ascending or descending
	int surface[6];         //!< start number for the edge faces
	int surface_incr_i[6];  //!< increment by going in the first parametric direction
	int surface_incr_j[6];  //!< increment by going in the second parametric direction
	int volume;             //!< internal volume starting number
} volGlobNumber;


/**********************************************************************************//**
 * \brief Main class for keeping track of the module topology, and properties
 *************************************************************************************/
class SplineModel {
	public:
		// constructors and destructors
		SplineModel();
		SplineModel(std::vector<boost::shared_ptr<Go::SplineSurface> > &spline_surfaces);
		SplineModel(std::vector<boost::shared_ptr<Go::SplineVolume> >  &spline_volumes);
		~SplineModel();

		// common get-functions
		TopologySet *getTopology();
		std::vector<boost::shared_ptr<Go::SplineVolume> > &getSplineVolumes();
		std::vector<boost::shared_ptr<Go::SplineSurface> > &getSplineSurfaces();
		
		// model geometry functions
		void setTopologyTolerance(double tol);
		bool enforceRightHandSystem();

		// local-to-global mapping functions
		void generateGlobalNumbers();
		void generateGlobalNumbersPETSc();

		// refinement schemes
		void knot_insert(int patchId, int parDir, double knot);
		void boundary_layer_refinement(int patchId, int parDir, bool start, double scale, int n);
		void uniform_h_refine();
		void uniform_p_refine();
		
		// model property functions
		bool addVolumePropertyCode(int volId, int propCode, bool inclusive=false);
		bool addFacePropertyCode(int volId, int faceId, int propCode, bool inclusive=true);
		bool addLinePropertyCode(int volId, int lineId, int propCode, bool inclusive=true);
		void addVertexPropertyCode(int volId, int vertId, int propCode);
		int getVolumePropertyCode(int volId);
		int getFacePropertyCode(int volId, int faceId);
		int getLinePropertyCode(int volId, int lineId);
		int getVertexPropertyCode(int volId, int vertId);
		bool isVolumetricModel() const;
		
		// input-/output-functions
		void writeSplines(std::ostream &os) const;
		void writeGlobalNumberOrdering(std::ostream &os) const;
		void writeModelProperties(std::ostream &os) const;
		void readSplines(std::istream &is);
		void readGlobalNumberOrdering(std::istream &is);
		void readModelProperties(std::istream &is);

	private:
		TopologySet *topology;
		bool volumetric_model;
		bool surface_model;
		volGlobNumber  *vl2g;
		surfGlobNumber *sl2g;
		std::vector<boost::shared_ptr<Go::SplineSurface> > spline_surfaces_; //!< Spline surface objects
		std::vector<boost::shared_ptr<Go::SplineVolume> >  spline_volumes_;  //!< Spline volume objects

};

#endif

