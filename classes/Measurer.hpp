#ifndef MEASURER_HPP
#define MEASURER_HPP
#include "Output.hpp"
#include "Eigen/Eigen"

using namespace std;
using namespace MainApplication;
using namespace Eigen;

namespace GeDiM
{
	class Measurer;
	class Measurer
	{
		public:
			enum TypeMeasurer
			{
				PointToPoint = 0,
				PointToLine = 1,
			};

		private:

			double coordinateProjectionOnLine;
			double distanceSquared;

			TypeMeasurer typeMeasurer;

		public:
			Measurer(const TypeMeasurer type);
			~Measurer();

			const Output::ExitCodes ComputeSquaredDistancePoints(const Vector3d& firstPoint, const Vector3d secondPoint);
			const Output::ExitCodes ComputeDistancePointLineAndProj(const Vector3d& point, const Vector3d& startPointLine, const Vector3d& tangentVector, const Vector3d& normalVector);

			const double DistanceSquared() { return distanceSquared; }
			const double CoordinateProjectionOnLine() { return coordinateProjectionOnLine; }
	};

}

#endif // MEASURER_HPP
