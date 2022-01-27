// Copyright (c) 2018 Nineva Studios

using System.IO;
using UnrealBuildTool;

public class AndroidAttachThing : ModuleRules
{
	public AndroidAttachThing(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] 
			{
#if UE_4_20_OR_LATER
				Path.Combine (ModuleDirectory, "Public")
#else
				"AndroidAttachThing/Public"
#endif
				// ... add public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] 
			{
#if UE_4_20_OR_LATER
				Path.Combine (ModuleDirectory, "Private")
#else
				"AndroidAttachThing/Private"
#endif
				// ... add other private include paths required here ...
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"Engine",
				"Core",
				"CoreUObject",
				"Settings"
				// ... add other public dependencies that you statically link with here ...
			}
		);

		// additional routine for Android
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
#if UE_4_20_OR_LATER
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));
#else
			PrivateIncludePaths.AddRange(
				new string[]
				{
					"AndroidAttachThing/Private/Android"
					// ... add other private include paths required here ...
				}
			);
#endif

			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

#if UE_4_19_OR_LATER
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "AndroidAttachThing_UPL.xml"));
#else
			AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "AndroidAttachThing_UPL.xml")));
#endif
		}
	}
}
