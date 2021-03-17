#include "DazToUnrealCommands.h"

#define LOCTEXT_NAMESPACE "FDazToUnrealModule"

void FDazToUnrealCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "DazToUnreal", "Bring up DazToUnreal window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(InstallDazStudioPlugin, "Install Daz Studio Plugin", "Install the Daz Studio plugin", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(InstallSkeletonAssets, "Install Skeleton Assets", "Install the Skeleton Assets to your project", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(InstallMaterialAssets, "Install Material Assets", "Install the Material Assets to your project", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
