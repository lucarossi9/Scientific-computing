#include "testBox.h"
#define YES 1
#define NO 0
#define DEBUG

namespace GeDiM
{
/*
enum coordinates
{
    x = 0,
    y = 1,
    z = 2
};

enum extreme
{
    compute_min = 1,
    compute_max = -1
};
*/

bool PointInPolygon (unsigned int&, const vector <Vector3d>&, int, int, const Vector3d&, const Vector3d&, const double& toll = 1.0E-7);
//double Compute_Extreme_Coordinates (const vector <Vector3d>& ,  extreme , coordinates , int, int, int, int, int);

testBox::testBox()
{
    //ctor
}

testBox::~testBox()
{
    //dtor
}


//COSTRUTTORE CON ARGOMENTI
testBox :: testBox(GenericCell* cell_input, const GenericDomain2D& polygon_input)
{
    cell = cell_input;
    polygon = &polygon_input;

    sign_test = NOT_DONE;
    sphere_test =  NOT_DONE;
    final_test = NOT_DONE;

    intersect2d1d.SetPlane(polygon_input.PlaneNormal(), polygon_input.PlaneTranslation()); // setto normale al piano del poligono e coefficiente di traslazione del piano del poligono


    // calcolo il vettore dei massimi e minimi del poligono per non ricalcolarlo ogni volta che chiamo sphere_test
    //in sphere test dovrò invece ricalcolare ogni volta quello del tetraedro

    int current_index=0, counter;
    const int N = polygon->NumberVertices();
    bool found; // flag = NO fino a quando non trovo il minimo nel ciclo while
    polyMin=polygon->Vertex(0);
    polyMax=polygon->Vertex(0);

    ////////////// calcolo minimi
    for (int j=0; j<3; j++)
    {
        found=NO;
        counter=0;
        while (found==NO && counter<N)
        {
        if ((polygon->Vertex(current_index)[j] < polygon->Vertex((current_index+1)%N)[j] && polygon->Vertex(current_index)[j] < polygon->Vertex((current_index+N-1)%N)[j]) ||
            ((polygon->Vertex(current_index)[j] < polygon->Vertex((current_index+1)%N)[j]) && (polygon->Vertex(current_index)[j] == polygon->Vertex((current_index+N-1)%N)[j])) ||
             ((polygon->Vertex(current_index)[j] < polygon->Vertex((current_index+N-1)%N)[j]) && (polygon->Vertex(current_index)[j] == polygon->Vertex((current_index+1)%N)[j])))
        {
            found = YES;
            polyMin[j] = polygon->Vertex(current_index)[j];
        }
        else
        {
            if (polygon->Vertex((current_index+1)%N)[j] <= polygon->Vertex((current_index+N-1)%N)[j])
            {
                current_index = (current_index+1)%N;
                counter++;
            }
            else
            {
                current_index = (current_index+N-1)%N;
                counter++;
            }
        }
        }
    }
     ////////////// calcolo massimi
    for (int j=0; j<3; j++)
    {
        found=NO;
        counter=0;
        while (found==NO && counter<N)
        {
        if ((polygon->Vertex(current_index)[j] > polygon->Vertex((current_index+1)%N)[j] && polygon->Vertex(current_index)[j] < polygon->Vertex((current_index+N-1)%N)[j]) ||
            ((polygon->Vertex(current_index)[j] > polygon->Vertex((current_index+1)%N)[j]) && (polygon->Vertex(current_index)[j] == polygon->Vertex((current_index+N-1)%N)[j])) ||
             ((polygon->Vertex(current_index)[j] > polygon->Vertex((current_index+N-1)%N)[j]) && (polygon->Vertex(current_index)[j] == polygon->Vertex((current_index+1)%N)[j])))
        {
            found = YES;
            polyMax[j] = polygon->Vertex(current_index)[j];
        }
        else
        {
            if (polygon->Vertex((current_index+1)%N)[j] <= polygon->Vertex((current_index+N-1)%N)[j])
            {
                current_index =(current_index+N-1)%N;
                counter++;
            }
            else
            {
                current_index = (current_index+1)%N;
                counter++;
            }
        }
        }
    }
    #ifdef DEBUG
    cout << "minimi :"<<endl;
    for (int i=0; i<3 ; i++)
    {
        cout<< polyMin[i]<<endl;
    }
    cout <<"massimi: " <<endl;
    for (int i=0; i<3 ; i++)
    {
        cout<< polyMax[i]<<endl;
    }
    #endif // DEBUG
}
//***********************************************************

//SETTER
void testBox :: SetCell(GenericCell* cell_input)
{
    //CLEAR ALL
    sign_test = NOT_DONE;
    sphere_test =  NOT_DONE;
    final_test = NOT_DONE;
    n_positive_points = 0;
    n_negative_points = 0;
    n_null_points = 0;
    positive_points.clear();
    negative_points.clear();
    null_points.clear();

    //settiamo la cella
    cell = cell_input;
}
//************************************************************************

const result testBox::ResultSignTest()
{
    if (sign_test == NOT_DONE)
        this->ComputeSignTest() ;

    return sign_test;
}
//***************************************************************************

const result testBox :: ResultSphereTest()
{
    if (sphere_test == NOT_DONE)
        this->ComputeSphereTest();

    return sphere_test;
}
//***************************************************************************

const result testBox :: ResultFinalTest()
{
    if (final_test == NOT_DONE)
        this->ComputeFinalTest();

    return final_test;
}
//***************************************************************************


//SIGN TEST////////////////////////////////////////////////////////////////////
const Output::ExitCodes testBox::ComputeSignTest( const double& toll)
{
    unsigned int i;
    n_negative_points = 0;
    n_positive_points = 0;
    n_null_points = 0;
    double dot_product;

#ifdef DEBUG
    cout << "sign test called" << endl;
#endif

   // controlli generici
    if (polygon->IsInitialized() == false)
    {
        cout << "Polygon is needed :" << polygon->GlobalId() ;
        return Output::GenericError;
    }
    if (toll<=0)
    {
        cout << "tolerance must be positive" ;
        return Output::GenericError;
    }

    for (i=0; i<4; i++)
        {
        //Prodotto scalare fra la normale al piano e il vettore che collega il centroide del poligono al vertice del tetraedro
        dot_product = polygon->PlaneNormal().dot(cell->Point(i)->Coordinates() - polygon->Centroid());

         if (dot_product > toll)
         {
             n_positive_points++;
             positive_points.push_back(cell->Point(i)->Coordinates());
         }
         else
         {
             if ((toll >= dot_product) && (dot_product >= -toll))
             {
                 n_null_points++;
                 null_points.push_back(cell->Point(i)->Coordinates());
             }
             else
             {
                 n_negative_points++;
                 negative_points.push_back(cell->Point(i)->Coordinates());
             }
         }
        }
    #ifdef DEBUG
        cout<< "positivi : "<< n_positive_points<< endl ;
        cout<< "negativi : "<< n_negative_points<< endl ;
        cout<< "nulli : "<< n_null_points<< endl ;
        #endif // DEBUG

    if (n_null_points == 4)
    {
        #ifdef DEBUG
        cout<< "tetraedro schiacciato in un piano"<< endl ;
        #endif // DEBUG
        sign_test =NOT_PASSED;
        return Output::GenericError;
    }

    if ((n_positive_points == 4) || (n_negative_points == 4))
    {
        sign_test = NOT_PASSED; // Non passo a fare i test, infatti il tetradro è completamente sopra o sotto al piano
        #ifdef DEBUG
        cout<< "test segni non passato"<< endl ;
        #endif // DEBUG
    }
    else
    {
        sign_test = PASSED; //  Passo a fare i test dopo
        #ifdef DEBUG
        cout<< "test segni passato"<< endl ;
        #endif // DEBUG
    }

    return Output::Success;
}
//*****************************************************************************



//SPHERE TEST /////////////////////////////////////////////////////////////////
const Output::ExitCodes testBox::ComputeSphereTest(const double& toll)
{
#ifdef DEBUG
cout << "sphere test called" << endl;
#endif

 ///// Controllo il poligono sia inizializzato
        if (polygon->IsInitialized()==false)
    {
        cout << "Polygon %u is needed" << polygon->GlobalId() ;
        return Output::GenericError;
    }

//// Controllo positività della tolleranza data
    if (toll<=0)
    {
        cout << "tolerance must be positive" ;
        return Output::GenericError;
    }

    Vector3d cellMin = cell->Point(0)->Coordinates();
    Vector3d cellMax = cell->Point(0)->Coordinates();

    //cerchiamo le 3 "dimensioni" della box che inscatola il tetraedro
    for (unsigned int i=1; i<cell->NumberOfPoints(); i++)
    {

        for (int j=0; j<3; j++)
        {
            if (cellMax[j]< cell->Point(i)->Coordinates()[j])
            {
                cellMax[j]=cell->Point(i)->Coordinates()[j];
            }

            if (cellMin[j]> cell->Point(i)->Coordinates()[j])
            {
                cellMin[j]=cell->Point(i)->Coordinates()[j];
            }
        }
    }

    //N.B: consideriamo buono anche il caso in cui non si intersecano di poco --> "differenza > -tol";
    ////// per ogni coordinata, controlliamo che:
    ////// (il max del poligono sia > del min della cella) && (il max della cella sia > del min del poligono) //per simmetria
    //////   ----------------
    //////             ---------------------
    ///////////////////////////////////////////////////////////
    //////    ----------------
    //////                         ---------------------  [in questo caso una delle due condizioni non è soddisfatta]
    if (((polyMax[0]-cellMin[0])>-toll) && ((polyMax[1]-cellMin[1])>-toll) && ((polyMax[2]-cellMin[2])>-toll) &&
         ((cellMax[0]-polyMin[0])>-toll) && ((cellMax[1]-polyMin[1])>-toll) && ((cellMax[2]-polyMin[2])>-toll))
    {
        sphere_test=PASSED;
        #ifdef DEBUG
        cout<< "test sfera passato"<< endl ;
        #endif // DEBUG
    }
    else
    {
        sphere_test=NOT_PASSED;
        #ifdef DEBUG
        cout<< "test sfera non passato"<< endl ;
        #endif // DEBUG
    }

 return Output::Success;

}
//**************************************************************************




//FINAL TEST////////////////////////////////////////////////////////////////
const Output::ExitCodes testBox::ComputeFinalTest(const double& toll)
{
    //declaration
    unsigned int i, j, counter, n;
    bool flag;
    Vector3d normal_section;

    #ifdef DEBUG
    cout << "final test called" << endl;
    #endif

    // #null points = #section points
    n_null_points = n_null_points + n_positive_points*n_negative_points; //each vertex is connected to every other vertex

        //computation of the vertices of the section, which are then put inside the null_points vector
        //REMARK: if n = 1 or n = 2, then nothing has to be done

        if(n_null_points == 3)
        {
            for(i=0; i<n_positive_points; i++)
            {

                for(j=0; j<n_negative_points; j++)
                {

                    intersect2d1d.SetLine( positive_points[i] , negative_points[j] - positive_points[i] ); //i segmenti che interseco sono quelli che hanno
                    //come origine un punto positivo e vettore tangente quello che li unisce a punti negativi

                    intersect2d1d.ComputeIntersection();

                    null_points.push_back(intersect2d1d.IntersectionPoint()); //inserisco i punti dell'intersezione (sezione) nel vettore con i punti nulli (in cui sono già presenti i punti della sezione)
                }
            }
        }


        else if(n_null_points == 4) // unica possibile configurazione 2 positivi e 2 negativi
        {
//////////////////////////////////// intersezione punto positivo 1 con punto negativo 1
                intersect2d1d.SetLine( positive_points[0] , negative_points[0] - positive_points[0] );

                intersect2d1d.ComputeIntersection();

                null_points.push_back(intersect2d1d.IntersectionPoint()); //inserisco il punto dell'intersezione (punto della sezione) nel vettore con i punti nulli (in cui sono già presenti i punti della sezione)

//////////////////////////////////// intersezione punto negativo 1 con punto positivo 2
                intersect2d1d.SetLine( negative_points[0] , positive_points[1] - negative_points[0] );

                intersect2d1d.ComputeIntersection();

                null_points.push_back(intersect2d1d.IntersectionPoint()); //inserisco il punto dell'intersezione (punto della sezione) nel vettore con i punti nulli (in cui sono già presenti i punti della sezione)

//////////////////////////////////// intersezione punto negativo 2 con punto positivo 2
                intersect2d1d.SetLine( positive_points[1] , negative_points[1] - positive_points[1] );

                intersect2d1d.ComputeIntersection();

                null_points.push_back(intersect2d1d.IntersectionPoint()); //inserisco il punto dell'intersezione (punto della sezione) nel vettore con i punti nulli (in cui sono già presenti i punti della sezione)

//////////////////////////////////// intersezione punto negativo 2 con punto positivo 1
                intersect2d1d.SetLine( negative_points[1] , positive_points[0] - negative_points[1] );

                intersect2d1d.ComputeIntersection();

                null_points.push_back(intersect2d1d.IntersectionPoint()); //inserisco il punto dell'intersezione (punto della sezione) nel vettore con i punti nulli (in cui sono già presenti i punti della sezione)
        }

        //CORE

        //for each point of the section, we check if it is inside the polygon
        flag = NO;
        i = 0;


       while((flag == NO) && (i < n_null_points))

        {
            n = 0;
            if((PointInPolygon(n, polygon->GetVertices(), 1, ((polygon->TotalNumberVertices())-1), polygon->PlaneNormal(), null_points[i])) == YES)
                flag = YES;

            #ifdef DEBUG
            cout << "Out of binary search!!!" << endl;
            #endif

            i++;
        }

        if(flag == YES) //se abbiamo trovato un punto interno, sappiamo già che c'è intersezione!!
            {
                final_test = PASSED;
                return Output::Success;
            }

        if(n_null_points == 1) //se abbiamo un solo punto non c'è altro da fare
            {
                final_test = NOT_PASSED;
                return Output::Success;
            }


        //*****************************************************************************************************************************************
        //INTERSEZIONI FRA OGNI COPPIA DI LATI
       i = 0;
       flag = NO;


       while((flag == NO) && (i < polygon->TotalNumberVertices()))
       {
           //facciamo il modulo per creare all'ultima iterazione il segmento (vertex[0] - vertex[n-1]), poichè vertex[0] = vertex[n % n]
           intersect1d1d.SetFirstTangentVector(polygon->RotatedVertex((i+1) % polygon->TotalNumberVertices()) - polygon->RotatedVertex(i));
           j = 0;

           while((flag == NO) && (j < n_null_points))
           {
               intersect1d1d.SetSecondTangentVector(polygon->RotatePoint(null_points[(j+1) % n_null_points]) - polygon->RotatePoint(null_points[j]));
               intersect1d1d.ComputeIntersectionEdges(polygon->RotatedVertex((i+1) % polygon->TotalNumberVertices()) - polygon->RotatedVertex(i),
                                                                       polygon->RotatePoint(null_points[(j+1) % n_null_points]) - polygon->RotatePoint(null_points[j]), polygon->RotatePoint(null_points[j]) - polygon->RotatedVertex(i));
               #ifdef DEBUG
               cout << "Type intersection " << intersect1d1d.TypeIntersection() << endl;
               #endif // DEBUG
               if ((intersect1d1d.TypeIntersection() ==  Intersector1D1D::IntersectionOnSegment) || (intersect1d1d.TypeIntersection() == Intersector1D1D::IntersectionParallelOnSegment))
                flag = YES;

               j++;
           }

           i++;
       }

       if(flag == YES)
       {
           final_test = PASSED;
           return Output::Success;
       }

       //REMARK: se i punti sono 2 non ha senso controllare se i punti del poligono sono interni alla sezione
       if(n_null_points == 2)
       {
           final_test = NOT_PASSED;
           return Output::Success;
       }



        //*****************************************************************************************************************************************

            //RECAP: se si arriva a questa sezione: n_null_points = 3,4  e NON ci sono intersezoni
            //Occorre controllare se tutti i vertici del poligono sono interni alla sezione.
            //Tuttavia, poichè non ci sono intersezioni, i vertici sono o tutti dentro o tutti fuori ==> basta controllarne uno solo!

            //non conoscendo se i vertici della sezione sono ordinati secondo la normale al poligono o al contrario mi calcolo la normale alla sezione
            if ((((null_points[1]-null_points[0]).cross(null_points[2]-null_points[0])).normalized()).dot(polygon->PlaneNormal()) >0) //in realtà questo prodotto scalare viene -1 o 1 (salvo inesattezze dovute alla precisione finita)
                normal_section = polygon->PlaneNormal();

            else
                normal_section = -polygon->PlaneNormal();

    counter=0;

            for( j=0 ; j<n_null_points ; j++) // controllo posizione primo punto del poligono rispetto alla sezione
            {
//conto quante volte il punto sta nei semipiani individuati dai lati della sezione comprendenti il poligono
                if ( (((null_points[(j+1)%n_null_points]-null_points[j]).normalized()).cross(polygon->Vertex(0) - null_points[j])).dot(normal_section)> -toll)
                    counter++;
            }

            if(counter == n_null_points)
            {
                final_test = PASSED;
                return Output::Success;
            }
            else
            {
                final_test = NOT_PASSED;
                return Output::Success;
            }


    }

//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************

//BINARY SEARCH /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//assumiamo che i vertici del poligono siano ordinati in verso antiorario rispetto alla normale
//abbiamo scelto di considerare interni anche punti che stanno praticamente sul segmento, ma leggermente fuori
 bool PointInPolygon (unsigned int& counter, const vector <Vector3d>& vertices, int dx, int sx, const Vector3d& normal, const Vector3d& point, const double& toll)
 {
     int mid;
     double mixedProduct, cross_sq_norm;

     #ifdef DEBUG
     cout << "binary search called" << endl;
     #endif

    //controllo che il punto stia nell'angolo individuato dai vettori: (vertices[1] - vertices[0]) e (vertices[0] - vertices[sx])
    if ( !((((( vertices[1]-vertices[0] ).normalized()).cross(point-vertices[0])).dot(normal) > -toll) && ((((vertices[0]-vertices[sx]).normalized()).cross(point-vertices[sx])).dot(normal) > -toll) ) && (counter==0) )
        return NO;

    counter ++;

     //BASE CASE
     if (sx - dx == 1)
     {
         #ifdef DEBUG
         cout << "binary search bottom reached!" << endl;
         #endif

        if ((((vertices[sx]-vertices[dx]).normalized()).cross(point-vertices[dx])).dot(normal) > -toll) // ho già controllato precedentemente che il punto appartega agli altri 2 semipiani
            //(delimitati dagli altri due lati del triangolo) in modo che sia interno
            return YES;
        else
            return NO;

     }

     //RECURSIVE STEP
     mid = floor((sx + dx)/2);
     mixedProduct = ((vertices[mid]-vertices[0]).cross(point-vertices[0])).dot(normal);
     cross_sq_norm = ((vertices[mid]-vertices[0]).cross(point-vertices[0])).dot(((vertices[mid]-vertices[0]).cross(point-vertices[0])));

     //caso in cui il punto giace sulla fascia del segmento (vertices[mid]-vertices[0])
     if (cross_sq_norm < toll*toll*((vertices[mid]-vertices[0]).dot((vertices[mid]-vertices[0]))))
     {

         if (((((vertices[mid+1]-vertices[mid]).normalized()).cross(point-vertices[mid])).dot(normal) > -toll)  && ((((vertices[mid]-vertices[mid-1]).normalized()).cross(point-vertices[mid-1])).dot(normal) > -toll)) // il
        // punto giace all'interno della sezione (anche se è poco fuori lo prendiamo dentro)
            return YES;
         else
            return NO;
     }

     if(mixedProduct > 0) //caso in cui il punto giace nel semipiano sinistro
        return PointInPolygon(counter, vertices, mid, sx, normal, point);
     else  //caso in cui il punto giace nel semipiano destro
        return PointInPolygon(counter, vertices, dx, mid, normal, point);
 }

 /*
///////////////////////////////////////////////////////////////////////////////////////////////
 // funzione per calcolo delle coordinate massime e minime di un poligono
 double Compute_Extreme_Coordinates (const vector <Vector3d>& vertices ,  extreme type_extreme , coordinates type_coordinate, int index_first_vertex, int sx, int dx, int number_of_vertices, int current_number_of_vertices )
 {
     double first_coordinate_vertex, mid_coordinate_vertex, tmp, result ;
     int temp;
     int index_mid_vertex=(sx+dx)/2;

     first_coordinate_vertex = type_extreme*(vertices[index_first_vertex][type_coordinate]) ;
     mid_coordinate_vertex = type_extreme*(vertices[index_mid_vertex][type_coordinate]);

     if (mid_coordinate_vertex < first_coordinate_vertex)
     {
         tmp = first_coordinate_vertex;
         first_coordinate_vertex = mid_coordinate_vertex;
         mid_coordinate_vertex = tmp;
         temp = index_mid_vertex;
         index_mid_vertex = index_first_vertex;
         index_first_vertex = temp;
     }

     if (type_extreme*(vertices[(index_first_vertex-1)%number_of_vertices][type_coordinate]) < first_coordinate_vertex )
     {
         result = Compute_Extreme_Coordinates(vertices, type_extreme, type_coordinate, index_first_vertex, index_mid_vertex, index_first_vertex, number_of_vertices );
     }
     if (type_extreme*(vertices[(index_first_vertex+1)%number_of_vertices][type_coordinate]) < first_coordinate_vertex )
     {
         result = Compute_Extreme_Coordinates(vertices, type_extreme, type_coordinate, index_first_vertex, index_first_vertex, index_mid_vertex, number_of_vertices );
     }

 }
 */



}




