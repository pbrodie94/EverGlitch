// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ProjectMoustache/ProjectMoustacheGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeProjectMoustacheGameMode() {}
// Cross Module References
	PROJECTMOUSTACHE_API UClass* Z_Construct_UClass_AProjectMoustacheGameMode_NoRegister();
	PROJECTMOUSTACHE_API UClass* Z_Construct_UClass_AProjectMoustacheGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_ProjectMoustache();
// End Cross Module References
	void AProjectMoustacheGameMode::StaticRegisterNativesAProjectMoustacheGameMode()
	{
	}
	UClass* Z_Construct_UClass_AProjectMoustacheGameMode_NoRegister()
	{
		return AProjectMoustacheGameMode::StaticClass();
	}
	struct Z_Construct_UClass_AProjectMoustacheGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AProjectMoustacheGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_ProjectMoustache,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AProjectMoustacheGameMode_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "ProjectMoustacheGameMode.h" },
		{ "ModuleRelativePath", "ProjectMoustacheGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AProjectMoustacheGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AProjectMoustacheGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AProjectMoustacheGameMode_Statics::ClassParams = {
		&AProjectMoustacheGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x008802ACu,
		METADATA_PARAMS(Z_Construct_UClass_AProjectMoustacheGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AProjectMoustacheGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AProjectMoustacheGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AProjectMoustacheGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AProjectMoustacheGameMode, 3264030129);
	template<> PROJECTMOUSTACHE_API UClass* StaticClass<AProjectMoustacheGameMode>()
	{
		return AProjectMoustacheGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AProjectMoustacheGameMode(Z_Construct_UClass_AProjectMoustacheGameMode, &AProjectMoustacheGameMode::StaticClass, TEXT("/Script/ProjectMoustache"), TEXT("AProjectMoustacheGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AProjectMoustacheGameMode);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
