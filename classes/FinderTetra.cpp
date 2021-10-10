#include "FinderTetra.hpp"
#include "testBox.h"
#include <queue>

#define YES 1
#define NO 0


namespace GeDiM
{
    FinderTetra::FinderTetra()
    {
        //ctor
    }

    FinderTetra::~FinderTetra()
    {
        //dtor
    }

    void FinderTetra::SetSkipTest2(bool b)
    {
       skipTest2 = b;

       #ifdef DEBUG
       cout << "Skip Test: " << b << endl;
       #endif // DEBUG
    }


    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    const Output::ExitCodes FinderTetra::FindCell(const GenericDomain2D& domain2D)
    {
        //FIRST PART: we look for an intersected tet ////////////////////////////////

        //declaration and initialization
        unsigned int i = 0;
        bool found = NO;
        int indexTet;
        testBox tester = testBox(meshPointer->Cell(0), domain2D);

        queue<GenericCell*> myQueue;
        GenericCell* myCell;

        //-----------------------------------------------------

        //research loop
        do
        {
            if (tester.ResultSignTest() == PASSED)
            {
                if (skipTest2 == YES)
                {
                    if (tester.ResultFinalTest() == PASSED)
                    {
                        found = YES;
                        indexTet = i;
                    }
                }
                else //we execute the sphere test
                {
                    if (tester.ResultSphereTest() == PASSED)
                    {
                        if (tester.ResultFinalTest() == PASSED)
                        {
                            found = YES;
                            indexTet = i;
                        }
                    }
                }
            }

            //disactivation (ho già visitato la cella e non voglio più visitarla)
            meshPointer->Cell(i)->SetState(false);

            i++;
            tester.SetCell(meshPointer->Cell(i));

        } while ((found == NO) && (i < meshPointer->NumberOfCells()));


    //----------------------------------------------------------------------

    //SECOND PART: queue ///////////////////////////////////////////////////
    if (found == NO)
     {
        Output::PrintWarningMessage("No cell intersected!", false);
        return Output::GenericError;
     }

    #ifdef DEBUG
    else
        cout << "FIRST CELL FOUND AT " << indexTet << endl;
    #endif
    myCell = meshPointer->Cell(indexTet);
    cellFound.push_back(myCell); //first tet put in the list
    for (i = 0; i < myCell->NumberOfCells(); i++) //neighbour cells put in the queue
        {
            if (myCell->Cell(i) != NULL) //cell restituisce null se la cella è sul bordo e il vicino quindi non esiste
            {
                if ((myCell->Cell(i))->IsActive() == true)
                {
                    meshPointer->Cell(myCell->Cell(i)->Id())->SetState(false); // l'id della cella è anche la posizione della stessa nella mesh
                    myQueue.push(meshPointer->Cell(myCell->Cell(i)->Id()));
                }
            }
        }
// ATTENTION: GenericCell.Cell() returns type CONST GenericCell*, whereas GenericMesh.Cell can return type NON-CONST GenericCell*

        while (!(myQueue.empty()))
        {
            myCell = myQueue.front();
            myQueue.pop();
            tester.SetCell(myCell);

            if (tester.ResultSignTest() == PASSED)
            {
                if (tester.ResultFinalTest() == PASSED)
                {
                    cellFound.push_back(myCell);

                    #ifdef DEBUG
                    cout << "CELL FOUND AT " << myCell->GlobalId() << endl;
                    #endif

                    // neighbour cells put in the queue
                    // se una cella non è già stata visitata e non ha vicini intersecati
                    //non verrà messa nella coda e controllata. In tal modo non controllo tutta la mesh
                    for (i = 0; i < myCell->NumberOfCells(); i++)
                    {
                         if(myCell->Cell(i) != NULL)
                        {
                            if(myCell->Cell(i)->IsActive())
                            {
                                meshPointer->Cell(myCell->Cell(i)->GlobalId())->SetState(false);
                                myQueue.push(meshPointer->Cell(myCell->Cell(i)->Id()));
                            }
                        }
                    }
                }
            }
        }
        #ifdef DEBUG
        cout << "Intersected cells: " << cellFound.size() << endl;
        #endif


        return Output::Success;
    }
}


