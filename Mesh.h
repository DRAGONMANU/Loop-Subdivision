#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include "Vector3D.h"
#include "Vector2D.h"

#include<algorithm>
using namespace std;

class Edge;
class Vertex;
class Face;
class Mesh;

class Edge
{
	public:
		int id, pair_id, next_id, vertex_id, face_id, nextEdgeVertex, texture_id, normal_id;
		Edge(int _id=-1,  int vertex=-1, int face=-1, int pair=-1 )
		{
			id=_id;
			vertex_id=vertex;
			nextEdgeVertex=-1;
			face_id=face;
			pair_id=pair;
			texture_id = -1;
		}

		void displayEdge();
		~Edge() {}
};

class Face{
	public:
		vector<int> ver_id;
		
		int id, edge_id;
		Edge faceEdge; //incident h-edge, first edge of a face
		Vector3D faceNormal;
		Vector3D faceCentroid;
		bool isNormal; // not needed
		Face( int d=0)
		{
			id=d;
		}
		void displayFace();
		void setFaceCentroid();
};

class Vertex
{
	public:
		Vector3D point;
		Vector3D normal; // normal is average of adjacent triangles, each weighted by angle of incident triangle edges
		int id, edge_id; // id of half edge from this vertex
		bool isNormal; // not needed
		bool isBoundary;
		Vertex(float x=0.0, float y=0.0, float z=0.0, int _id=0, int edge=-1)
		{
			id=_id;
			edge_id=edge;
			Vector3D xx(x,y,z);
			point=xx;
			isNormal=false;
			isBoundary=false;
		}
		void displayVertex();

};

class Mesh
{
	public:
		vector<Face> faces;
		vector<Vertex> vertices;
		vector<Edge> edges;
		vector<Vector2D> texture_mapping;
		vector<Vector3D> normal_mapping;
		Mesh() {}
		void loadFile(char *fname);
		void displayMesh();
		void setFaceNormal();
		int previousEdge(int edge);
		vector<int> edgesOfVertex(int vertex);
		vector<int> edgesOfFace(int face);
		void setFaceCentroid();
};

Vertex getLoopEdgeVertex(Mesh , int edge_id);
vector<Vertex> getLoopVertices(Mesh); 
void setPairEdge(Mesh &);
void setVertexNormal(Mesh &);
void StringSplit(string str, string separator, vector<string>* results);
Mesh getLoopSub(Mesh);
