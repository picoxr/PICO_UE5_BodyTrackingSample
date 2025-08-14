//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#include "PXR_DPEditorStyle.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FPICOXRDPEditorStyle::StyleInstance = NULL;

void FPICOXRDPEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPICOXRDPEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FPICOXRDPEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PICOXRDPEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);


TSharedRef< FSlateStyleSet > FPICOXRDPEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PICOXRDPEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PICOLivePreview")->GetBaseDir() / TEXT("Resources"));

	Style->Set("PICOXRDPEditor.PluginAction", new IMAGE_BRUSH(TEXT("Icon_Connected"), Icon40x40));
	Style->Set("PICOXRDPEditor.PluginAction2", new IMAGE_BRUSH(TEXT("Icon_Disconnected"), Icon40x40));
	Style->Set("PICOXRDPEditor.PluginAction.Small", new IMAGE_BRUSH(TEXT("Icon_Connected"), Icon16x16));
	Style->Set("PICOXRDPEditor.PluginAction2.Small", new IMAGE_BRUSH(TEXT("Icon_Disconnected"), Icon16x16));

	return Style;
}



#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FPICOXRDPEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FPICOXRDPEditorStyle::Get()
{
	return *StyleInstance;
}
