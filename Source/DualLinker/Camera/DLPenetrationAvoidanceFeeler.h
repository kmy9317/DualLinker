#pragma once

#include "CoreMinimal.h"

#include "DLPenetrationAvoidanceFeeler.generated.h"

/**
 * Struct defining a feeler ray used for camera penetration avoidance.
 */
USTRUCT()
struct FDLPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	/** feeler마다 서로 다른 방향(예, 좌측, 우측, 위, 아래 등)으로 Ray를 쏘도록 함 */
	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceFeeler)
	FRotator AdjustmentRot;

	/** 이 feeler의 결과가 월드(즉, static geometry)에 충돌했을 때 차단 결과에 얼마나 큰 영향을 미칠지를 결정하는 가중치 */
	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceFeeler)
	float WorldWeight;

	/** 이 feeler가 Pawn(예: 다른 캐릭터나 플레이어)의 충돌과 관련된 경우에 적용되는 가중치, 0으로 하면 Pawn과의 충돌 보정X */
	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceFeeler)
	float PawnWeight;

	/** 트레이스의 민감도와 충돌 영역의 크기를 결정 */
	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceFeeler)
	float Extent;

	/** feeler를 언제 다시 트레이스할지(매 프레임마다 검사하지 않고 일정한 간격으로 검사) 제어 */
	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceFeeler)
	int32 TraceInterval;

	UPROPERTY(transient)
	int32 FramesUntilNextTrace;


	FDLPenetrationAvoidanceFeeler()
		: AdjustmentRot(ForceInit)
		, WorldWeight(0)
		, PawnWeight(0)
		, Extent(0)
		, TraceInterval(0)
		, FramesUntilNextTrace(0)
	{
	}

	FDLPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
		const float& InWorldWeight,
		const float& InPawnWeight,
		const float& InExtent,
		const int32& InTraceInterval = 0,
		const int32& InFramesUntilNextTrace = 0)
		: AdjustmentRot(InAdjustmentRot)
		, WorldWeight(InWorldWeight)
		, PawnWeight(InPawnWeight)
		, Extent(InExtent)
		, TraceInterval(InTraceInterval)
		, FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};