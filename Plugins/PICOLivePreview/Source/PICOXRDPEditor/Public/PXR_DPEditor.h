//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "IDetailCustomization.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "Input/Reply.h"
#include "Textures/SlateIcon.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWidget;

class FPICOXRDPEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** These functions will be bound to Commands */
	void PluginButtonClicked();
	void OnConnectStateChanged(int32 flag);
	FSlateIcon GetImage();

	void RegisterSettings();
	void UnregisterSettings();
private:
	bool ShowLivePreviewToolbarDropdown();
	void BindConnectionDelegate();

	int32 CurrentTryTimes=0;
	bool bRequestConnectToPDC=false;
	bool bDisconnectToPDC=false;
	bool bConnected =false;
	TSharedPtr<class FUICommandList> PluginCommands;
};
