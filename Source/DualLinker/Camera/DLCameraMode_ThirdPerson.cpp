// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"
#include "DLCameraAssistInterface.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "DrawDebugHelpers.h"


namespace DLCameraMode_ThirdPerson_Statics
{
	static const FName NAME_IgnoreCameraCollision = TEXT("IgnoreCameraCollision");
}

UDLCameraMode_ThirdPerson::UDLCameraMode_ThirdPerson()
{
	TargetOffsetCurve = nullptr;

	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f), 1.00f, 1.00f, 14.f, 0));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+00.0f, +16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+00.0f, -16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+00.0f, +32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+00.0f, -32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(+20.0f, +00.0f, 0.0f), 1.00f, 1.00f, 00.f, 4));
	PenetrationAvoidanceFeelers.Add(FDLPenetrationAvoidanceFeeler(FRotator(-20.0f, +00.0f, 0.0f), 0.50f, 0.50f, 00.f, 4));
}

void UDLCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// TargetOffsetCurve가 존재시 Curve에 값을 가져와서 적용 진행
	// - Camera 관점에서 Charater의 어느 부분을 Target으로 할지 결정
	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}

	UpdatePreventPenetration(DeltaTime);
}

void UDLCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
	if (!bPreventPenetration)
	{
		return;
	}

	AActor* TargetActor = GetTargetActor();

	APawn* TargetPawn = Cast<APawn>(TargetActor);
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	IDLCameraAssistInterface* TargetControllerAssist = Cast<IDLCameraAssistInterface>(TargetController);
	IDLCameraAssistInterface* TargetActorAssist = Cast<IDLCameraAssistInterface>(TargetActor);

	// TODO: 캐릭터의 특정 부위나 다른 Target들을 PreventPenetrationTarget로 지정할 수 있다
	TOptional<AActor*> OptionalPPTarget = TargetActorAssist ? TargetActorAssist->GetCameraPreventPenetrationTarget() : TOptional<AActor*>();
	AActor* PPActor = OptionalPPTarget.IsSet() ? OptionalPPTarget.GetValue() : TargetActor;
	IDLCameraAssistInterface* PPActorAssist = OptionalPPTarget.IsSet() ? Cast<IDLCameraAssistInterface>(PPActor) : nullptr;

	const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent());
	if (PPActorRootComponent)
	{
		// SafeLocation 계산 시작:
		// 1) 기본적으로 PPActor의 ActorLocation을 사용.
		FVector SafeLocation = PPActor->GetActorLocation();
		FVector ClosestPointOnLineToCapsuleCenter;

		// 2) View.Rotation.Vector()와 View.Location(카메라 위치)를 기준으로,
		//    PPActor의 중심에서 가장 가까운 점(캡슐 중심에 가까운 지점)을 구함.
		FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

		// 3) PushInDistance 계산: 
		// PenetrationAvoidanceFeelers[0].Extent와 CollisionPushOutDistance를 더한 값
		float const PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;

		// 4) PPActor의 캡슐 기본 높이(절반)에서 PushInDistance를 뺀 값을 MaxHalfHeight로 사용.
		float const MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;

		// 5) Z축 보정: SafeLocation.Z를, ClosestPointOnLineToCapsuleCenter.Z를 중심으로, 
		//    SafeLocation.Z의 상/하 범위(MaxHalfHeight)를 사용해 Clamp.
		SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

		// 6) 충돌 감지를 위한 SquaredDistance 계산:
		// PPActorRootComponent->GetSquaredDistanceToCollision()를 호출하여, 
		// ClosestPointOnLineToCapsuleCenter과 컴포넌트의 충돌 Shape 간의 가장 가까운 거리와 표면 위치를 계산
		float DistanceSqr;
		PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr, SafeLocation);

		// 7) 만약 PenetrationAvoidanceFeelers 배열에 항목이 있다면,
		//    SafeLocation를 (SafeLocation - ClosestPointOnLineToCapsuleCenter)의 방향으로
		//    PushInDistance만큼 밀어 넣어 카메라가 충돌되기 전에 안전하게 유지되도록 조정.
		if (PenetrationAvoidanceFeelers.Num() > 0)
		{
			SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
		}

		// 이후, SafeLocation을 기준으로 실제 카메라 위치와의 충돌 여부를 체크하고 카메라 위치 보정
		bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
		PreventCameraPenetration(*PPActor, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct, bSingleRayPenetrationCheck);

		// 여러 Assist 객체를 통해 카메라 관통 경고를 보낼 수 있음.
		IDLCameraAssistInterface* AssistArray[] = { TargetControllerAssist, TargetActorAssist, PPActorAssist };

		if (AimLineToDesiredPosBlockedPct < ReportPenetrationPercent)
		{
			for (IDLCameraAssistInterface* Assist : AssistArray)
			{
				if (Assist)
				{
					Assist->OnCameraPenetratingTarget();
				}
			}
		}
	}
}

void UDLCameraMode_ThirdPerson::PreventCameraPenetration(AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly)
{
	// 메인 feeler 차단 비율
	float HardBlockedPct = DistBlockedPct;
	// 주변 feeler들의 평균적 차단 비율
	float SoftBlockedPct = DistBlockedPct;

	FVector BaseRay = CameraLoc - SafeLoc;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;

	BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

	float DistBlockedPctThisFrame = 1.f;

	int32 const NumRaysToShoot = bSingleRayOnly ? FMath::Min(1, PenetrationAvoidanceFeelers.Num()) : PenetrationAvoidanceFeelers.Num();
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr/*PlayerCamera*/);

	SphereParams.AddIgnoredActor(&ViewTarget);

	//TODO 카메라 Penetration 관련 ignore할 엑터들 선별 기능
	// IDLCameraTarget.GetIgnoredActorsForCameraPentration();
	//if (IgnoreActorForCameraPenetration)
	//{
	//	SphereParams.AddIgnoredActor(IgnoreActorForCameraPenetration);
	//} 

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
	UWorld* World = GetWorld();

	for (int32 RayIdx = 0; RayIdx < NumRaysToShoot; ++RayIdx)
	{
		FDLPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIdx];
		if (Feeler.FramesUntilNextTrace <= 0)
		{
			// calc ray target
			FVector RayTarget;
			{
				FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
				RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
				RayTarget = SafeLoc + RotatedRay;
			}

			SphereShape.Sphere.Radius = Feeler.Extent;
			ECollisionChannel TraceChannel = ECC_Camera;

			// do multi-line check to make sure the hits we throw out aren't
			// masking real hits behind (these are important rays).

			// TODO: 추후 IgnoreActor와 그 뒤 충돌 Actor등에 대한 상황으로 인해 SweepMultiByChannel() 사용 고려
			FHitResult Hit;
			const bool bHit = World->SweepSingleByChannel(Hit, SafeLoc, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);

			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			const AActor* HitActor = Hit.GetActor();

			if (bHit && HitActor)
			{
				bool bIgnoreHit = false;

				// 엑터에 IgnoreCameraCollision 태그 있으면 ignore로 등록
				if (HitActor->ActorHasTag(DLCameraMode_ThirdPerson_Statics::NAME_IgnoreCameraCollision))
				{
					bIgnoreHit = true;
					SphereParams.AddIgnoredActor(HitActor);
				}

				// CameraBlockingVolume인 경우 별도 처리하여, 카메라의 전방에서 발생하는 충돌은 무시
				if (!bIgnoreHit && HitActor->IsA<ACameraBlockingVolume>())
				{
					// ViewTarget 기준 전방 확인
					const FVector ViewTargetForwardXY = ViewTarget.GetActorForwardVector().GetSafeNormal2D();
					const FVector ViewTargetLocation = ViewTarget.GetActorLocation();
					const FVector HitOffset = Hit.Location - ViewTargetLocation;
					const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();
					const float DotHitDirection = FVector::DotProduct(ViewTargetForwardXY, HitDirectionXY);
					if (DotHitDirection > 0.0f)
					{
						bIgnoreHit = true;
						// Ignore this CameraBlockingVolume on the remaining sweeps.
						SphereParams.AddIgnoredActor(HitActor);
					}
				}

				if (!bIgnoreHit)
				{
					// Weight 적용: Pawn인지 World인지에 따라 가중치 다르게 적용
					float const Weight = Cast<APawn>(Hit.GetActor()) ? Feeler.PawnWeight : Feeler.WorldWeight;
					float NewBlockPct = Hit.Time;
					// Hit.Time (충돌시 발생한 충돌 비율)과 가중치를 혼합해 새로운 차단 비율 계산
					NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

					// 실제 PushOut 처리를 고려하여, SafeLoc로부터 Hit.Location 사이의 비율 계산
					NewBlockPct = ((Hit.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).Size();
					DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

					// 이번 feeler에서는 히트가 발생했으므로, 다음 프레임에 또 트레이스하도록 설정
					Feeler.FramesUntilNextTrace = 0;
				}
			}

			if (RayIdx == 0)
			{
				// 중앙의 feeler (가장 중요한 Ray)는 차단 비율을 그대로 사용
				HardBlockedPct = DistBlockedPctThisFrame;
			}
			else
			{
				// 나머지 feeler는 보조적 역할 - 소프트하게 비율 적용
				SoftBlockedPct = DistBlockedPctThisFrame;
			}
		}
		else
		{
			--Feeler.FramesUntilNextTrace;
		}
	}

	if (bResetInterpolation)
	{
		DistBlockedPct = DistBlockedPctThisFrame;
	}
	else if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		// 부드럽게 증가(Out) 시킨다.
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame - DistBlockedPct);
		}
		else
		{
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	else
	{
		if (DistBlockedPct > HardBlockedPct)
		{
			DistBlockedPct = HardBlockedPct;
		}
		else if (DistBlockedPct > SoftBlockedPct)
		{
			// 부드럽게 감소(In) 시킨다.
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct - SoftBlockedPct);
			}
			else
			{
				DistBlockedPct = SoftBlockedPct;
			}
		}
	}

	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);
	if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
	}
}


