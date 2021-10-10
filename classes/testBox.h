#ifndef TESTBOX_H
#define TESTBOX_H

#include "GenericMesh.hpp"
#include "GenericDomain.hpp"
#include "Output.hpp"
#include "Intersector2D1D.hpp"
#include "Intersector1D1D.hpp"

#include <vector>

#define DEBUG

enum result
{
    PASSED = 1,
    NOT_PASSED = -1,
    NOT_DONE = 0
};



namespace GeDiM
{

class testBox
{
    private:
    GenericCell* cell;
    const GenericDomain2D* polygon;

    result sign_test, sphere_test, final_test;

    //needed for sign_test
    unsigned int n_positive_points, n_negative_points, n_null_points;

    //needed for final test
    vector<Vector3d> positive_points, negative_points, null_points;
    Intersector2D1D intersect2d1d;
    Intersector1D1D intersect1d1d;

    // needed for sphere_test
    //polymin è un vector 3D contenente l'ascissa, l'ordinata e l'altezza minima del poligono, polymax quelle massime
    Vector3d polyMin, polyMax;



    public:
        testBox();
        testBox(GenericCell* cell_input, const GenericDomain2D& polygon_input);
        virtual ~testBox();

        void SetCell(GenericCell* cell_input);

        const result ResultSignTest();
        const result ResultSphereTest();
        const result ResultFinalTest();

        // restituisce un output generic error se non andato a buon fine e success altrimenti.
        const Output::ExitCodes ComputeSignTest( const double& toll  = 1.0E-7);
        const Output::ExitCodes ComputeSphereTest(const double& toll  = 1.0E-7);
        const Output::ExitCodes ComputeFinalTest(const double& toll  = 1.0E-7);



};
}
#endif // TESTBOX_H
