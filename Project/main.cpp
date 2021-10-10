#include "GenericDomain.hpp"
#include "GenericMesh.hpp"
#include "Domain3D.hpp"
#include "MeshImport_Tetgen.hpp"
#include "FinderTetra.hpp"
#include "Cutter3D.hpp"
#include <iostream>

//valori di SkipTest2
#define YES 1
#define NO 0

using namespace GeDiM;
using namespace Eigen;

int main(int argc, char** argv)
{
	/// CREATE DOMAIN
	unsigned int id = 1;
    Parallelepiped domain(id);
    Vector3d origin(0.0, 0.0, 0.0), length(1.0, 0.0, 0.0), height(0.0, 0.0, 1.0), width(0.0, 1.0, 0.0);

    domain.BuildParallelepiped(origin, length, height, width);

    MeshImport_Tetgen meshImportTetgen;
    meshImportTetgen.SetMinimumNumberOfCells(1);
    GenericMesh mesh;
    meshImportTetgen.CreateTetgenInput(domain);
    meshImportTetgen.CreateTetgenOutput(domain);
    meshImportTetgen.CreateMesh(domain, mesh);
    Output::PrintGenericMessage("Number of cell before the cutter %d", true, mesh.NumberOfCells());
    /// CREATE DOMAIN
	//const unsigned int numDomainVertices = 4 ;
	const unsigned int numDomainVertices = 6;
	GenericDomain2D domain2D(0,numDomainVertices);
	vector<Vector3d> vertexCoords(numDomainVertices);


	char garbage;
    ifstream InFile; /* Dichiarazione di tipo */
    InFile.open ("esempio_4.txt"); /* Apertura del file */
    if (!InFile)
    {
        cout << "Errore di apertura del file" << endl; /* controllo */
    }
    else
    {
       for(unsigned int i=0 ;i<numDomainVertices; i++)
       {
           for(unsigned int j=0; j<3; j++)
           {
               InFile >> vertexCoords[i][j];
               InFile >> garbage;
           }
       }
    }
    InFile.close();
/*
esempio 1
	vertexCoords[0] << 0.2, 0.2, 0.5;
	vertexCoords[1] << 0.5, 0.2, 0.5;
	vertexCoords[2] << 0.5, 0.5, 0.5;
	vertexCoords[3] << 0.2, 0.5, 0.5;

esempio 2
    vertexCoords[0] << 1.0, 0.7, 0.0;
	vertexCoords[1] << 1.0, 0.75, 0.0;
	vertexCoords[2] << 1.0, 0.75, 0.05;
    vertexCoords[3] << 1.0, 0.7, 0.05;

esempio 3
    vertexCoords[0] << 0.5, 0.7, 0.4;
	vertexCoords[1] << 0.7, 0.7, 0.8;
	vertexCoords[2] << 0.7, 0.5, 0.8;
    vertexCoords[3] << 0.5, 0.5, 0.4;

esempio esagono
    vertexCoords[0] << 0.4, 0.4, 0.6;
	vertexCoords[1] << 0.6, 0.4, 0.6;
	vertexCoords[2] << 0.7, 0.5, 0.6;
    vertexCoords[3] << 0.6, 0.7, 0.6;
    vertexCoords[4] << 0.4, 0.7, 0.6;
    vertexCoords[5] << 0.3, 0.5, 0.6;
*/

	for(unsigned int i = 0; i < numDomainVertices; i++)
	{
		domain2D.AddVertex(vertexCoords[i]);
		domain2D.AddEdge(i, (i+1)%numDomainVertices);
    }
	domain2D.Initialize();

/*
    mesh.ComputeGeometricalProperties();
	CutterMesh3D cutter;
	cutter.SetMesh(mesh);
	cutter.Initialize(1);
	cutter.AddDomain(domain2D);
	cutter.CutMesh();
	mesh.CleanInactiveTreeNode();
	Output::PrintGenericMessage("Number of cell after the cutter %d", true, mesh.NumberOfCells());
*/

	/// FINDER TETRA
	FinderTetra finder;
	finder.SetMesh(mesh);

	//SKIP_TEST_2
	///////////////////////////////////////////////////////////
    bool flag = YES;
    unsigned int i = 0;

    while(flag==YES && i<domain2D.TotalNumberVertices())
       {
           //controllo se esiste un vertice del poligono all'interno del parallelepipedo in cui vive la mesh, in tal caso eseguo anche il test 2
           if((domain2D.Vertex(i).x()>origin.x() && domain2D.Vertex(i).x()<length.x()) && (domain2D.Vertex(i).y()>origin.y() && domain2D.Vertex(i).y()<width.y()) && (domain2D.Vertex(i).z()>origin.z() && domain2D.Vertex(i).z()<height.z()))
            flag = NO;
           i++;
       }
    finder.SetSkipTest2(flag);

    ////////////////////////////////////////////////////////////
	finder.FindCell(domain2D);

	const list<GenericCell*> cellFound = finder.CellFound();
	/// OUTPUT MESH TO MATLAB SCRIPT FOR VISUALIZATION
	ofstream file("plotCellFound.m", ofstream::out );
	if(file.is_open())
	{
        file << "nodes = [";
        for(unsigned int i = 0; i < mesh.NumberOfPoints(); i++)
            file << mesh.Point(i)->Coordinates() (0) << "," <<  mesh.Point(i)->Coordinates() (1) << "," <<  mesh.Point(i)->Coordinates() (2) << ";" << endl;
        file << "];" << endl;

        file << "connectivity = [";
        for(list<GenericCell*>::const_iterator it = cellFound.begin(); it != cellFound.end(); it++)
        {
            GenericCell& cell = *(*it);
            file << cell.Point(0)->Id()+1 << "," <<  cell.Point(1)->Id()+1 << "," << cell.Point(2)->Id()+1 << "," << cell.Point(3)->Id()+1 << ";" << endl;
        }
        file << "];" << endl;
        file << "tetramesh(connectivity, nodes);" << endl;
        //aggiunta nostra (stampa il poligono)
        file << "hold on"<< endl;
        for(unsigned int i=0; i<numDomainVertices; i++)
        {
            file << "v"<<i<<"=["<<vertexCoords[i][0]<<","<<vertexCoords[i][1]<<","<<vertexCoords[i][2]<<"]"<<endl;
        }
        for(unsigned int i=0; i<numDomainVertices; i++)
        {
            file<< "s"<<i<<"=[v"<<i<<"; v"<<(i+1)%numDomainVertices<<"]"<<endl;
        }
        for(unsigned int i=0; i<numDomainVertices; i++)
        {
            file<<"plot3(s"<<i<<"(:,1), s"<<i<<"(:,2), s"<<i<<"(:,3), 'k') "<<endl;
        }
        file.close();
	}
	else
        Output::PrintErrorMessage("Unable to open the file", true);

}
