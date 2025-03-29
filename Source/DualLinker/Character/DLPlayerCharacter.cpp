// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ADLPlayerCharacter::ADLPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->TargetArmLength = 400.0f;  
	SpringArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
	SpringArm->bUsePawnControlRotation = true;  

	// 카메라 생성 (SpringArm 끝에 부착)
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;  
	Camera->FieldOfView = 90.0f; 
}

void ADLPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADLPlayerCharacter::Move(const FVector2D& InputVector)
{
	if (GetController())
	{
		const FRotator MovementRotation(0.0f, GetController()->GetControlRotation().Yaw, 0.0f);

		if (InputVector.X != 0.0f)
		{
			// Left/Right -> X 값에 들어있음:
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput 함수를 한번 보자:
			// - 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용(더하기)해준다
			AddMovementInput(MovementDirection, InputVector.X);
		}

		if (InputVector.Y != 0.0f) // 앞서 우리는 Forward 적용을 위해 swizzle input modifier를 사용했다~
		{
			// 앞서 Left/Right와 마찬가지로 Forward/Backward를 적용한다
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, InputVector.Y);
		}
	}
}

void ADLPlayerCharacter::Look(const FVector2D& InputVector)
{

	if (InputVector.X != 0.0f)
	{
		// X에는 Yaw 값이 있음:
		// - Camera에 대해 Yaw 적용
		AddControllerYawInput(InputVector.X);
	}

	if (InputVector.Y != 0.0f)
	{
		// Y에는 Pitch 값!
		double AimInversionValue = -InputVector.Y;
		AddControllerPitchInput(AimInversionValue);
	}
}

