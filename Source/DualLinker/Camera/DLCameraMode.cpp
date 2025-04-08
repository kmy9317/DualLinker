// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCameraMode.h"
#include "Components/CapsuleComponent.h"
#include "DLCameraComponent.h"
#include "GameFramework/Character.h"
#include "DLPlayerCameraManager.h"

FDLCameraModeView::FDLCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(DL_CAMERA_DEFAULT_FOV)
{
}

void FDLCameraModeView::Blend(const FDLCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	// Location + OtherWeight * (Other.Location - Location);
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	// Location과 같은 방식 Lerp (ControlRotation과 FieldOfView도 같음)
	// GetNormalized()를 사용하여 각도를 -!80 ~ 180으로 유지(최소의 회전 보장)
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

UDLCameraMode::UDLCameraMode()
{
	FieldOfView = DL_CAMERA_DEFAULT_FOV;
	ViewPitchMin = DL_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = DL_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.5f;
	BlendFunction = EDLCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

void UDLCameraMode::UpdateCameraMode(float DeltaTime)
{
	// Actor를 활용하여, CameraMode Pivot의 Location 및 Rotation을 계산하여 View를 업데이트
	UpdateView(DeltaTime);

	// BlendWeight를 DeltaTime을 활용하여, BlendAlpha 계산 후, BlendFunction에 맞게 재-매핑하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UDLCameraMode::UpdateView(float DeltaTime)
{
	// CameraMode를 가지고 있는 CameraComponent의 Owner인 Character을 활용하여, PivotLocation/Rotation을 반환함
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch 값에 대해 Min/Max를 Clamp시킴
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FDLCameraModeView에 PivotLocation/Rotation 설정
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivotRotation을 똑같이 ControlRotation으로 활용
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// 즉 Character의 Location과 ControlRotation을 활용하여, View를 업데이트
}

void UDLCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	switch (BlendFunction)
	{
	case EDLCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case EDLCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EDLCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EDLCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UDLCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

	switch (BlendFunction)
	{
	case EDLCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	case EDLCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EDLCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EDLCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

UDLCameraComponent* UDLCameraMode::GetDLCameraComponent() const
{
	// - GetOuter(): DLCameraComponent -> DLCameraModeStack
	// - GetOuter(): DLCameraComponent -> DLCameraMode(GetCameraModeInstance에서 DLCameraModeStack의 Outer를 DLCameraMode의 Outer로 지정)
	return CastChecked<UDLCameraComponent>(GetOuter());
}

UWorld* UDLCameraMode::GetWorld() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* UDLCameraMode::GetTargetActor() const
{
	const UDLCameraComponent* DLCameraComponent = GetDLCameraComponent();
	return DLCameraComponent->GetTargetActor();
}

FVector UDLCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// Crouching상태 고려하여 높이 조정
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}
		// BaseEyeHeight를 고려하여, ViewLocation을 반환함
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UDLCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// 보통 Pawn의 ControlRotation을 반환
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

UDLCameraModeStack::UDLCameraModeStack()
{

}

UDLCameraMode* UDLCameraModeStack::GetCameraModeInstance(TSubclassOf<UDLCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstances에서 먼저 생성되어있는지 확인 후, 반환한다:
	for (UDLCameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode는 UClass를 비교한다:
		// - 즉, CameraMode는 클래스 타입당 한개씩 생성
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass에 알맞는 CameraMode의 인스턴스가 없다면 생성한다:
	UDLCameraMode* NewCameraMode = NewObject<UDLCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// CameraModeInstances는 CameraModeClass에 맞는 인스턴스를 관리하는 컨테이너
	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UDLCameraModeStack::PushCameraMode(TSubclassOf<UDLCameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}
	UDLCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// CameraModeStack[0] 가장 최근에 이미 CameraMode가 Stacking되었으므로 그냥 리턴
		return;
	}

	// ExistingStackIndex는 CameraModeStack에서 CameraMode에 맞는 Index를 찾음
	// ExistingStackContribution은 위에서 아래로 최종 BlendWeight 값을 찾기 위해 초기값으로 1.0으로 설정
	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	// BlendWeight는 쉽게 이해하자면 블렌딩이 경과된 비율이고 ExistingStackCOntribution는 카메라들의 경과 비율에 대한 합을 1.0으로 유지하기 위한 공식
	/**
	 * BlendWeight    |    ExistingStackCOntribution    |    ExistingStackCOntribution (accumulated)
	 * 0.1f           |    (1.0f) * 0.1f = 0.1f         |    (1.0f - 0.1f) = 0.9f
	 * 0.3f           |    (0.9f) * 0.3f = 0.27f        |    (1.0f - 0.3f) * 0.9f = 0.63f
	 * 0.6f           |    (0.63f) * 0.6f = 0.378f      |    (1.0f - 0.6f) * 0.63f = 0.252f
	 * 1.0f           |    (0.252f) * 1.0f = 0.252f     |
	 *                |    0.1f + 0.27f + 0.378f + 0.252f = 1.0f!
	 */
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}

	// CameraMode를 Top으로 반영하기 위해 제거하여 다시 Push
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		ExistingStackContribution = 0.0f;
	}

	// BlendTime이 0보다 크다는 것은 Blend을 얼마 시간동안 진행함을 의미 따라서, ExistingStackContribution을 적용
	// - 따라서 Blend하지 않는다면 BlendWeight를 1.0을 넣어 새로 넣는 CameraMode만 적용할 것이다
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->SetBlendWeight(BlendWeight);

	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UDLCameraModeStack::EvaluateStack(float DeltaTime, FDLCameraModeView& OutCameraModeView)
{
	// Top(최신) -> Bottom [0 -> Num]까지 순차적으로 Stack에 있는 CameraMode 업데이트
	UpdateStack(DeltaTime);

	// Bottom -> Top까지 CameraModeStack에 대해 Blending 진행
	BlendStack(OutCameraModeView);
}

void UDLCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack을 순회하며, CameraMode를 업데이트한다
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UDLCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		CameraMode->UpdateCameraMode(DeltaTime);

		// CameraMode의 BlendWeight가 1.0에 도달했다면, 그 이후 CameraMode를 제거
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UDLCameraModeStack::BlendStack(FDLCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraMode를 Bottom -> Top 순서로 Blend를 진행
	const UDLCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// 최하단 CameraMode는 BlendWeight가 1로 고정
	OutCameraModeView = CameraMode->View;
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}
