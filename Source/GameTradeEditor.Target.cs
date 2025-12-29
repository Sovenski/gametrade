// Copyright Medieval Trading Dynasty. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameTradeEditorTarget : TargetRules
{
	public GameTradeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("GameTrade");
	}
}
