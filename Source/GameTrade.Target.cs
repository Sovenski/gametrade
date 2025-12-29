// Copyright Medieval Trading Dynasty. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameTradeTarget : TargetRules
{
	public GameTradeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GameTrade");
	}
}
