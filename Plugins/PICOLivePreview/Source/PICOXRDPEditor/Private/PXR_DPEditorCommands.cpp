//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#include "PXR_DPEditorCommands.h"

#define LOCTEXT_NAMESPACE "FPICOXREditorModule"
/* UI_COMMAND takes long for the compile to optimize */
UE_DISABLE_OPTIMIZATION_SHIP
void FPICOXRDPEditorCommands::RegisterCommands()
{
 	UI_COMMAND(PluginAction, "PICOXRDPEditor", "Execute PICOXRDPEditor action", EUserInterfaceActionType::Button, FInputChord());
 	UI_COMMAND(ConnectToServer, "ConnectToServer", "ConnectToServer", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DisconnectToServer, "DisconnectToServer", "DisconnectToServer", EUserInterfaceActionType::Button, FInputChord());
 
}
UE_ENABLE_OPTIMIZATION_SHIP

#undef LOCTEXT_NAMESPACE
