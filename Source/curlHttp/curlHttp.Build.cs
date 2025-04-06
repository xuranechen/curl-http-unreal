// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class curlHttp : ModuleRules
{
	//模块路径
	private string ModulePath
	{
		//获取build.cs的目录
		get { return ModuleDirectory; }
	}

	public curlHttp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
		//头文件路径
		string CURLIncludePath = ModulePath + "/libcurl-x64/include/curl/";
		//lib路径
		string CURLLibPath = ModulePath + "/libcurl-x64/lib/";
		//引入lib名称
		string CURLImportLibName = ModulePath + "/libcurl-x64/lib/libcurl.lib";

		// 添加头文件的包含路径
		PublicIncludePaths.Add(CURLIncludePath);
		//添加静态库方法路径
		PublicSystemLibraryPaths.Add(CURLLibPath);
		//附加库列表
		//PublicAdditionalLibraries.Add(CURLImportLibName);
	}
}