//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PXR_DPSettings.generated.h"


UENUM(BlueprintType)
enum class EGraphicQuality : uint8
{
	High,
	Medium,
	Low
};
/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class PICOXRDPHMD_API UPICOXRDPSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(Config, EditAnywhere, Category = "PICOXR LivePreview Graphic", Meta = (DisplayName = "Display Graphic Quality",ConfigRestartRequired=true))
	EGraphicQuality GraphicQuality;
};
