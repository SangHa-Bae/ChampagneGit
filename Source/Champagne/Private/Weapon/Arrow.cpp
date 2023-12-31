// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Arrow.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/PickUpInterface.h"
#include "PlayerController/ChamPlayerController.h"

#include "Enemy/Enemy.h"


// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent = Root;
	//Root->bVisualizeComponent = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(GetRootComponent());
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ArrowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ArrowBox"));
	ArrowBox->SetupAttachment(ArrowMesh);
	ArrowBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ArrowBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArrowBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ArrowBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	SphereArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereArea"));
	SphereArea->SetupAttachment(ArrowMesh);

	ArrowTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ArrowTrail"));
	ArrowTrail->SetupAttachment(ArrowMesh);

	ArrowMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ArrowMovement"));	
	ArrowMovement->InitialSpeed = 1000.f;
	ArrowMovement->MaxSpeed = 6500.f;	
	ArrowMovement->bRotationFollowsVelocity = true;	

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(ArrowMesh);
	PickupWidget->SetVisibility(false);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void AArrow::HighlightArrow(bool Switch)
{
	if (ArrowMesh == nullptr)
		return;

	if (Switch == true)
	{
		ArrowMesh->SetRenderCustomDepth(true);
		ArrowMesh->SetCustomDepthStencilValue(1);
	}
	else
	{
		ArrowMesh->SetRenderCustomDepth(false);
	}
}

void AArrow::ShowPickupWidget(bool Switch)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(Switch);
	}
}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
	
	//BoxCollision->OnComponentHit.AddDynamic(this, &AArrow::BoneHit);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AArrow::WhenHit);

	SphereArea->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnSphereAreaBeginOverlap);
	SphereArea->OnComponentEndOverlap.AddDynamic(this, &AArrow::OnSphereAreaEndOverlap);
	
	if (ArrowTrail->GetFXSystemAsset())
	{
		ArrowTrail->Activate();
	}
}

void AArrow::WhenHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor && OtherActor != this && OtherActor != GetOwner() && !OtherActor->IsA<AArrow>())
	{	
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy)
		{
			AChamPlayerController* ChamController = Cast<AChamPlayerController>(GetInstigatorController());

			if (ChamController && SweepResult.BoneName != FName("None"))
			{
				if (SweepResult.BoneName == Enemy->GetWeakBone())
				{
					ChamController->PlayHitMarker(false);
				}
				else
				{
					ChamController->PlayHitMarker(true);
				}

				UE_LOG(LogTemp, Warning, TEXT("BodyIndex : %d"), OtherBodyIndex);
				UE_LOG(LogTemp, Warning, TEXT("BoneName : %s"), *SweepResult.BoneName.ToString());
			}		
		}

		ArrowMovement->StopMovementImmediately();
		ArrowMovement->ProjectileGravityScale = 0.f;		
		AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FVector ParticleLocation = BoxCollision->GetComponentToWorld().GetLocation();
		UWorld* World = GetWorld();
		if (World && HitParticle && ArrowTrail)
		{			
			UGameplayStatics::SpawnEmitterAtLocation(World, HitParticle, ParticleLocation);
			ArrowTrail->Deactivate();
		}
	}
}

void AArrow::OnSphereAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor);

	if (PickUpInterface)
	{
		PickUpInterface->SetOverlappingArrow(this);
	}
}

void AArrow::OnSphereAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor);

	if (PickUpInterface)
	{
		PickUpInterface->RemoveOverlappingArrow(this);
	}
}

void AArrow::SetArrowSpeed(float ArrowSpeed)
{
	ArrowMovement->InitialSpeed = ArrowSpeed;
}
