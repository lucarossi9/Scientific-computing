#include "Measurer.hpp"

namespace GeDiM
{
	// ***************************************************************************
	Measurer::Measurer(const TypeMeasurer type)
	{
		typeMeasurer = type;
		coordinateProjectionOnLine = 0.0;
		distanceSquared = 0.0;
	}

	Measurer::~Measurer()
	{

	}

	const Output::ExitCodes Measurer::ComputeSquaredDistancePoints(const Vector3d& firstPoint, const Vector3d secondPoint)
	{
		if(typeMeasurer != Measurer::PointToPoint)
		{
			Output::PrintErrorMessage("Type Measurer does not coincide ", false);
			return Output::GenericError;
		}
		distanceSquared = (secondPoint-firstPoint).squaredNorm();
		return Output::Success;
	}

	// ***************************************************************************
	const Output::ExitCodes  Measurer::ComputeDistancePointLineAndProj(const Vector3d& point, const Vector3d& startPointLine, const Vector3d& tangentVector, const Vector3d& normalVector)
	{
		if(typeMeasurer != Measurer::PointToLine)
		{
			Output::PrintErrorMessage("Type Measurer does not coincide ", false);
			return Output::GenericError;
		}

		const Vector3d tangentVectorDifference = point - startPointLine;
		distanceSquared = tangentVectorDifference.dot(normalVector);
		distanceSquared *= distanceSquared;
		coordinateProjectionOnLine = tangentVector.dot(tangentVectorDifference) / tangentVector.squaredNorm();
		return Output::Success;
	}
}
