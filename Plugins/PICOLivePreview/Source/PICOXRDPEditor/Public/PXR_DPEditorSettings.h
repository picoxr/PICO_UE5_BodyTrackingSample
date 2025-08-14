//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PXR_DPEditorSettings.generated.h"

/**
 * 
 */
UCLASS(config = Editor, defaultconfig)
class PICOXRDPEDITOR_API UPICOXRDPEditorSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/** Whether or not to show the PICOXR Input settings toolbar button */
	UPROPERTY(config, EditAnywhere, Category = "PICOXR LivePreview Settings")
	bool bShowLivePreviewToolbarButton;

};
