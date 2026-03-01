#include "Commands/EpicUnrealMCPBuildCommands.h"
#include "Modules/ModuleManager.h"

// ILiveCodingModule is a developer-only module — include only in editor builds.
#if WITH_EDITOR
#include "ILiveCodingModule.h"
#endif

// ---------------------------------------------------------------------------

FEpicUnrealMCPBuildCommands::FEpicUnrealMCPBuildCommands()
{
}

TSharedPtr<FJsonObject> FEpicUnrealMCPBuildCommands::HandleCommand(
	const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
	if (CommandType == TEXT("trigger_live_coding"))
	{
		return HandleTriggerLiveCoding(Params);
	}
	return nullptr;
}

TSharedPtr<FJsonObject> FEpicUnrealMCPBuildCommands::HandleTriggerLiveCoding(
	const TSharedPtr<FJsonObject>& Params)
{
	TSharedPtr<FJsonObject> Result = MakeShareable(new FJsonObject);

#if WITH_EDITOR
	if (!FModuleManager::Get().IsModuleLoaded(TEXT("LiveCoding")))
	{
		Result->SetBoolField(TEXT("success"), false);
		Result->SetStringField(TEXT("error"), TEXT("LiveCoding module is not loaded"));
		return Result;
	}

	ILiveCodingModule& LC = FModuleManager::GetModuleChecked<ILiveCodingModule>(TEXT("LiveCoding"));

	if (!LC.HasStarted())
	{
		Result->SetBoolField(TEXT("success"), false);
		Result->SetStringField(TEXT("error"), TEXT("Live Coding console has not started"));
		return Result;
	}

	// Trigger a non-blocking compile. Returns immediately with InProgress (started)
	// or an error code. The Python side polls the log file for the final result.
	ELiveCodingCompileResult CompileResult = ELiveCodingCompileResult::Failure;
	LC.Compile(ELiveCodingCompileFlags::None, &CompileResult);

	FString ResultStr;
	switch (CompileResult)
	{
	case ELiveCodingCompileResult::Success:            ResultStr = TEXT("Success");            break;
	case ELiveCodingCompileResult::NoChanges:          ResultStr = TEXT("NoChanges");          break;
	case ELiveCodingCompileResult::InProgress:         ResultStr = TEXT("InProgress");         break;
	case ELiveCodingCompileResult::CompileStillActive: ResultStr = TEXT("CompileStillActive"); break;
	case ELiveCodingCompileResult::NotStarted:         ResultStr = TEXT("NotStarted");         break;
	case ELiveCodingCompileResult::Failure:            ResultStr = TEXT("Failure");            break;
	case ELiveCodingCompileResult::Cancelled:          ResultStr = TEXT("Cancelled");          break;
	default:                                           ResultStr = TEXT("Unknown");            break;
	}

	const bool bStarted = (CompileResult == ELiveCodingCompileResult::InProgress);

	Result->SetBoolField(TEXT("success"), true);
	Result->SetBoolField(TEXT("started"), bStarted);
	Result->SetStringField(TEXT("compile_result"), ResultStr);
#else
	Result->SetBoolField(TEXT("success"), false);
	Result->SetStringField(TEXT("error"), TEXT("trigger_live_coding is only available in Editor builds"));
#endif

	return Result;
}