#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Handler class for build-related MCP commands.
 * Supports triggering Live Coding (hot reload) with log capture.
 */
class UNREALMCP_API FEpicUnrealMCPBuildCommands
{
public:
	FEpicUnrealMCPBuildCommands();

	TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
	TSharedPtr<FJsonObject> HandleTriggerLiveCoding(const TSharedPtr<FJsonObject>& Params);
};