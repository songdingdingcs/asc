// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CurserHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CurserHit);
	if (!CurserHit.bBlockingHit) return;
	
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CurserHit.GetActor()); 
	
	/**
	 * 光标的Line trsce 有多种情况
	 * A. LastActor和ThisActor都是 null
	 *   -不做操作
	 * B.LastActor是null，ThhisActor是valid
	 *   -highlight thisactor
	 * C.LastActor是valid ThisActor 是null
	 *   -UnHighLight LastActor
	 * D.两个actor都是valid 但是两个actor不透
	 *   -UnHighLight LastActor 同时highlight thisactor
	 * E.两个actor都是valid 但是两个actor相同
	 *   -不做操作
	 */

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//CASE B
			ThisActor->HighlightActor();
		}
		else
		{
			//CASE A - both actor are null  do nothing
			
		}
	}
	else //LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			//case C
			LastActor->UnHighlightActor();
		}
		else // both actor are valid
		{
			if (LastActor != ThisActor)
			{
				//cASE D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//cASE E  DO NOTHING
				 
			}
		}
		
	}
	
}

void AAuraPlayerController::BeginPlay()
	{
		Super::BeginPlay();
		check(AuraContext);
	
	 
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		check(Subsystem);
		Subsystem->AddMappingContext(AuraContext,0);
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent*  EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation  = GetControlRotation();
	const FRotator YawRotation( 0.f,Rotation.Yaw,0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
	
}



