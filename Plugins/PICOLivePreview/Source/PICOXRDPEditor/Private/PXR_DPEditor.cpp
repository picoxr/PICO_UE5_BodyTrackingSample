//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#include "PXR_DPEditor.h"
#include "IMotionController.h"
#include "Features/IModularFeatures.h"
#include "LevelEditor.h"
#include "Engine/Engine.h"
#include "ISettingsModule.h"
#include "PXR_DPEditorSettings.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Input/SButton.h"
#include "DetailWidgetRow.h"
#include "PXR_DPEditorCommands.h"
#include "Runtime/Core/Public/CoreGlobals.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "ToolMenus.h"
#include "PXR_DPManager.h"
#include "PXR_DPEditorStyle.h"
#include "PXR_Log.h"


static const FName PICOXREditorTabName("PICOXREditor");

#define LOCTEXT_NAMESPACE "FPICOXREditorModule"
#define PICO_TRY_TIMES 3

void FPICOXRDPEditorModule::StartupModule()
{
	RegisterSettings();

	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FPICOXRDPEditorModule::BindConnectionDelegate);

	FPICOXRDPEditorStyle::Initialize();

	FPICOXRDPEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	// Dummy action for main toolbar button
	PluginCommands->MapAction(
		FPICOXRDPEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FPICOXRDPEditorModule::PluginButtonClicked),
		FCanExecuteAction());
	FPICOXRDPEditorStyle MenuStyle = FPICOXRDPEditorStyle();
	MenuStyle.Initialize();

	UToolMenu* ToolBar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = ToolBar->FindOrAddSection("PluginTools");

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"LivePreview",
		FUIAction(
			FExecuteAction::CreateRaw(this, &FPICOXRDPEditorModule::PluginButtonClicked),
			FCanExecuteAction(),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateRaw(this, &FPICOXRDPEditorModule::ShowLivePreviewToolbarDropdown)),
		LOCTEXT("LivePreviewBtn", "Live Preview Link"),
		LOCTEXT("LivePreviewBtnTootlip", "Show LivePreviewLink status,Clicked to Link PDC"),
		TAttribute<FSlateIcon>::Create(TAttribute<FSlateIcon>::FGetter::CreateRaw(this, &FPICOXRDPEditorModule::GetImage))
	));
}

bool FPICOXRDPEditorModule::ShowLivePreviewToolbarDropdown()
{
	// Only add the toolbar if PICOXR is the currently active tracking system
	UPICOXRDPEditorSettings* PICOXREditorSettings = GetMutableDefault<UPICOXRDPEditorSettings>();

	if (PICOXREditorSettings->bShowLivePreviewToolbarButton)
	{
		return true;
	}

	return false;
}

FOnConnectFromServiceEvent OnConnectFromServiceEvent;

void FPICOXRDPEditorModule::BindConnectionDelegate()
{
	OnConnectFromServiceEvent.BindRaw(this, &FPICOXRDPEditorModule::OnConnectStateChanged);
	FPICOXRDPManager::SetDisconnectFromServiceEvent(OnConnectFromServiceEvent);
}

void FPICOXRDPEditorModule::ShutdownModule()
{
	FPICOXRDPEditorStyle::Shutdown();

	FPICOXRDPEditorCommands::Unregister();
}

void FPICOXRDPEditorModule::PluginButtonClicked()
{
	bRequestConnectToPDC = true;
	CurrentTryTimes = 0;
}

void FPICOXRDPEditorModule::OnConnectStateChanged(int32 flag)
{
	bConnected = flag ? true : false;
}

FSlateIcon FPICOXRDPEditorModule::GetImage()
{
	if (FPICOXRDPManager::IsConnectToServiceSucceed())
	{
		if (bRequestConnectToPDC)
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview reconnect to the server succeeded!");
			bRequestConnectToPDC = false;
			bDisconnectToPDC = false;
			CurrentTryTimes = 0;
		}

		return FSlateIcon(FPICOXRDPEditorStyle::GetStyleSetName(), "PICOXRDPEditor.PluginAction");
	}
	else
	{
		if (bRequestConnectToPDC && CurrentTryTimes < PICO_TRY_TIMES)
		{
			FPICOXRDPManager::DisConnectStreamingServer();
			FPICOXRDPManager::ConnectStreamingServer();
			CurrentTryTimes++;
			PXR_LOGD(PxrUnreal, "PXR_LivePreview Try to connect to the server:%d times", CurrentTryTimes);
		}
		return FSlateIcon(FPICOXRDPEditorStyle::GetStyleSetName(), "PICOXRDPEditor.PluginAction2");
	}
}


void FPICOXRDPEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Editor", "PICOXRLivePreview Settings",
			LOCTEXT("PICOXRLivePreviewSettingsName", "PICOXRLivePreview Settings"),
			LOCTEXT("PICOXRLivePreviewSettingsDescription", "Configure the PICOXRLivePreview plugin"),
			GetMutableDefault<UPICOXRDPEditorSettings>()
		);

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	}
}

void FPICOXRDPEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "PICOXR");
	}
}


IMPLEMENT_MODULE(FPICOXRDPEditorModule, PICOXRDPEditor)

#undef LOCTEXT_NAMESPACE
