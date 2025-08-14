//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once

#include "PXR_DPEditorStyle.h"


class FPICOXRDPEditorCommands : public TCommands<FPICOXRDPEditorCommands>
{
public:

	FPICOXRDPEditorCommands()
		: TCommands<FPICOXRDPEditorCommands>(TEXT("PICOXRDPEditor"), NSLOCTEXT("Contexts", "PICOXRDPEditor", "PICOXRDPEditor Plugin"), NAME_None, TEXT("PICOXRDPEditor.Common.Icon"))
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> PluginAction;
	TSharedPtr<FUICommandInfo> ConnectToServer;
	TSharedPtr<FUICommandInfo> DisconnectToServer;

};
