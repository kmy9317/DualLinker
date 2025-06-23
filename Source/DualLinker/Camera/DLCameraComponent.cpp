// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCameraComponent.h"

#include "DLCameraMode.h"

UDLCameraComponent::UDLCameraComponent() : CameraModeStack(nullptr)
{

}

void UDLCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		// 카메라 컴포넌트를 지금 생성한 CameraModeStack의 Outer로 지정
		CameraModeStack = NewObject<UDLCameraModeStack>(this);
	}
}

void UDLCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	UpdateCameraModes();

	FDLCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			// PlayerController의 ControlRotation을 계산된 CamerModeView의 ControlRotation으로 업데이트
			// 해당 함수는 PC가 Possess하고 있는 Pawn의 RootComponent의 ControlRotation을 반영(조종 캐릭에 반영)
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// Camera의 Location과 Rotation을 반영
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	// FOV 업데이트
	FieldOfView = CameraModeView.FieldOfView;

	 // FMinimalViewInfo는 엔진이 카메라를 렌더링하는데 필요한 모든 기본 정보를 포함하고 있는 핵심 구조체
	 // - CameraComponent의 변화 사항을 반영해서 최종 렌더링까지 반영하게 됨
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void UDLCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	if (DetermineCameraModeDelegate.IsBound())
	{
		if (TSubclassOf<UDLCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}

