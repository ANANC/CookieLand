// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapTypes.h"

// -- FCookieLandLocation --

#pragma region FCookieLandLocation

bool FCookieLandLocation::GetIsMaxByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		return Floor > Other.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		return X > Other.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		return Y > Other.Y;
		break;
	};
	return false;
}

bool FCookieLandLocation::GetIsMaxEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		return Floor >= Other.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		return X >= Other.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		return Y >= Other.Y;
		break;
	};
	return false;
}

bool FCookieLandLocation::GetIsMinByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		return Floor < Other.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		return X < Other.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		return Y < Other.Y;
		break;
	};
	return false;
}

bool FCookieLandLocation::GetIsMinEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		return Floor <= Other.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		return X <= Other.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		return Y <= Other.Y;
		break;
	};
	return false;
}

void FCookieLandLocation::AddDistanceByThreeDirection(ECookieLandPieceOrientation Orientation, int32 Distance)
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		Floor += Distance;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		X += Distance;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		Y += Distance;
		break;
	};
}

void FCookieLandLocation::AddDistanceBySixDirection(ECookieLandPieceOrientation Orientation, int32 Distance)
{
	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
		Floor += Distance;
		break;
	case ECookieLandPieceOrientation::Down:
		Floor -= Distance;
		break;
	case ECookieLandPieceOrientation::Left:
		X += Distance;
		break;
	case ECookieLandPieceOrientation::Right:
		X -= Distance;
		break;
	case ECookieLandPieceOrientation::Forward:
		Y += Distance;
		break;
	case ECookieLandPieceOrientation::Backward:
		Y -= Distance;
		break;
	};
}

TArray<ECookieLandPieceOrientation> FCookieLandLocation::GetRelativeOrientations(FCookieLandLocation Location) const
{
	TArray<ECookieLandPieceOrientation> RelativeOrientations;

	if (Location.X > X)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Right);
	}
	else if (Location.X < X)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Left);
	}

	if (Location.Y > Y)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Forward);
	}
	else if (Location.Y < Y)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Backward);
	}

	if (Location.Floor > Floor)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Up);
	}
	else if (Location.Floor < Floor)
	{
		RelativeOrientations.Add(ECookieLandPieceOrientation::Down);
	}

	return RelativeOrientations;
}

TMap<ECookieLandPieceOrientation, int> FCookieLandLocation::GetRelativeOrientationAndDistances(FCookieLandLocation Location) const
{
	TMap<ECookieLandPieceOrientation, int> RelativeOrientationAndDistances;

	if (Location.X > X)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Right, Location.X - X);
	}
	else if (Location.X < X)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Left, X - Location.X);
	}

	if (Location.Y > Y)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Forward, Location.Y - Y);
	}
	else if (Location.Y < Y)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Backward, Y - Location.Y);
	}

	if (Location.Floor > Floor)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Up, Location.Floor - Floor);
	}
	else if (Location.Floor < Floor)
	{
		RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Down, Floor - Location.Floor);
	}

	return RelativeOrientationAndDistances;
}

TMap<ECookieLandPieceOrientation, int> FCookieLandLocation::GetAbsRelativeDistances(FCookieLandLocation Location) const
{
	TMap<ECookieLandPieceOrientation, int> RelativeDistancess;

	RelativeDistancess.Add(ECookieLandPieceOrientation::Left, abs(X - Location.X));
	RelativeDistancess.Add(ECookieLandPieceOrientation::Forward, abs(Y - Location.Y));
	RelativeDistancess.Add(ECookieLandPieceOrientation::Up, abs(Floor - Location.Floor));

	return RelativeDistancess;
}

#pragma endregion


// -- FCookieLandOrientationLinkInfo --

#pragma region FCookieLandOrientationLinkInfo

void FCookieLandOrientationLinkInfo::SetData(ECookieLandPieceOrientation InOrientation, FCookieLandLocation InMax, FCookieLandLocation InMin)
{
	bValid = true;
	Max_PieceLocation = InMax;
	Min_PieceLocation = InMin;
	Orientation = InOrientation;
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::SetMax(FCookieLandLocation InMax)
{
	Max_PieceLocation = InMax;
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::SetMin(FCookieLandLocation InMin)
{
	Min_PieceLocation = InMin;
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::AddLocation(FCookieLandLocation Location)
{
	if (Location.GetIsMinByOrientation(Orientation, Min_PieceLocation))
	{
		SetMin(Location);
	}
	else if (Location.GetIsMaxByOrientation(Orientation, Max_PieceLocation))
	{
		SetMax(Location);
	}
}

void FCookieLandOrientationLinkInfo::UpdateDistance()
{
	Distance = 0;

	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		Distance = Max_PieceLocation.Floor - Min_PieceLocation.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		Distance = Max_PieceLocation.X - Min_PieceLocation.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		Distance = Max_PieceLocation.Y - Min_PieceLocation.Y;
		break;
	};
}

void FCookieLandOrientationLinkInfo::ClearData()
{
	Max_PieceLocation = FCookieLandLocation();
	Min_PieceLocation = FCookieLandLocation();
	Distance = 0;
	bValid = false;
}

bool FCookieLandOrientationLinkInfo::GetIsInSide(FCookieLandLocation Location)
{
	if (Location.GetIsMaxEqualByOrientation(Orientation, Min_PieceLocation) && Location.GetIsMinEqualByOrientation(Orientation, Max_PieceLocation))
	{
		return true;
	}
	return false;
}

#pragma endregion