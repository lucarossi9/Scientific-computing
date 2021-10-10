#ifndef FINDERTETRA_HPP
#define FINDERTETRA_HPP

#include "GenericMesh.hpp"
#include "GenericDomain.hpp"
#include "Output.hpp"
#include "Eigen/Eigen"

namespace GeDiM
{
    class FinderTetra
    {
        private:

            GenericMesh* meshPointer;
            list<GenericCell*> cellFound;
            bool skipTest2; //AGGIUNTA

        public:
            FinderTetra();
            virtual ~FinderTetra();

            void SetMesh(GenericMesh& mesh) {meshPointer = &mesh;}
            void SetSkipTest2(bool b); //AGGIUNTA

            //const Output::ExitCodes FindCell(const GenericDomain2D& domain2D) { return Output::UnimplementedMethod; }
            const Output::ExitCodes FindCell(const GenericDomain2D& domain2D);
            const list<GenericCell*>& CellFound() const {return cellFound;}

    };
}

#endif // FINDERTETRA_HPP
