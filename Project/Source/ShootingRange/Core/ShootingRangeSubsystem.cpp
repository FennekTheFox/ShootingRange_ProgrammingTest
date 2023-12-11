#include "ShootingRangeSubsystem.h"

int32 UShootingRangeSubsystem::GetScore()
{
	return Score;
}

void UShootingRangeSubsystem::IncreaseScore(int32 Amount)
{
	if (Amount != 0)
	{
		Score += Amount;
		OnScoreChanged.Broadcast();
	}
}

