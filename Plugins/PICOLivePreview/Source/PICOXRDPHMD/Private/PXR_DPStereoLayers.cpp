//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#include "CoreMinimal.h"
#include "StereoLayerManager.h"
#include "PXR_DPHMD.h"
#include "Misc/ScopeLock.h"
#include "DefaultXRCamera.h"
#include "PXR_Log.h"


#if STEAMVR_SUPPORTED_PLATFORMS

//=============================================================================
void SetLayerDescMember(FPICODPLayer& Layer, const IStereoLayers::FLayerDesc& InLayerDesc)
{
	PXR_LOGD(PxrUnreal,"PXR_LivePreview SetLayerDescMember Layer ID:%d",InLayerDesc.Id);
	if (InLayerDesc.Texture != Layer.LayerDesc.Texture)
	{
		Layer.bUpdateTexture = true;
	}
	Layer.LayerDesc = InLayerDesc;
}

//=============================================================================
bool GetLayerDescMember(const FPICODPLayer& Layer, IStereoLayers::FLayerDesc& OutLayerDesc)
{
	PXR_LOGD(PxrUnreal,"PXR_LivePreview GetLayerDescMember Layer ID:%d",Layer.GetLayerId());
	OutLayerDesc = Layer.LayerDesc;
	return true;
}

//=============================================================================
void MarkLayerTextureForUpdate(FPICODPLayer& Layer)
{
	PXR_LOGD(PxrUnreal,"PXR_LivePreview MarkLayerTextureForUpdate Layer ID:%d", Layer.GetLayerId());
	Layer.bUpdateTexture = true;
}



//=============================================================================


#endif //STEAMVR_SUPPORTED_PLATFORMS
