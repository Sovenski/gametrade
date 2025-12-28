// Copyright Medieval Trading Dynasty. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameTradeEditorTarget : TargetRules
{
	public GameTradeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("GameTrade");
	}
}
