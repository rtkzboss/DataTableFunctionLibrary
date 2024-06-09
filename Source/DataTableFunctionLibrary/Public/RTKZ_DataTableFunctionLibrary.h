#pragma once
#include "CoreMinimal.h"
#include "Engine/CompositeDataTable.h"
#include "RTKZ_DataTableFunctionLibrary.generated.h"

#define LOCTEXT_NAMESPACE "RTKZ_DataTableFunctionLibrary"

UCLASS()
class DATATABLEFUNCTIONLIBRARY_API URTKZ_DataTableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DataTable")
	static void AppendParentDataTables(UCompositeDataTable* DataTable, TArray<UDataTable*> const& NewTables);

	UFUNCTION(BlueprintCallable, Category = "DataTable")
	static void RemoveDataTableRow(UDataTable* DataTable, FName RowName);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "DataTable", meta = (CustomStructureParam = "RowData"))
	static void AddDataTableRow(UDataTable* DataTable, FName RowName, FTableRowBase const& RowData);

	DECLARE_FUNCTION(execAddDataTableRow)
	{
		P_GET_OBJECT(UDataTable, DataTable);
		P_GET_PROPERTY(FNameProperty, RowName);

		Stack.StepCompiledIn<FStructProperty>(NULL);
		void* RowPtr = Stack.MostRecentPropertyAddress;
		FStructProperty* RowProp = CastField<FStructProperty>(Stack.MostRecentProperty);
		P_FINISH;

		if (!DataTable)
		{
			FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, LOCTEXT("AddDataTableRow_InvalidTable", "AddDataTableRow: The table is invalid."));
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
			return;
		}
		if (!RowProp || !RowPtr)
		{
			FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, LOCTEXT("AddDataTableRow_MissingInputProperty", "AddDataTableRow: The row struct is invalid."));
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
			return;
		}
		UScriptStruct const* InputType = RowProp->Struct;
		UScriptStruct const* TableType = DataTable->GetRowStruct();
		if (!RowTypeCompatibleWithTable(InputType, TableType))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("InputType"), FText::AsCultureInvariant(GetPathNameSafe(InputType)));
			Args.Add(TEXT("TableType"), FText::AsCultureInvariant(GetPathNameSafe(TableType)));
			FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, FText::Format(LOCTEXT("AddDataTableRow_Incompatible", "AddDataTableRow: The row struct {InputType} is incompatible with the table row struct {TableType}."), Args));
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
			return;
		}
		P_NATIVE_BEGIN;
		DataTable->AddRow(RowName, *static_cast<FTableRowBase*>(RowPtr));
		P_NATIVE_END;
	}

private:
	static bool RowTypeCompatibleWithTable(UScriptStruct const* InputType, UScriptStruct const* TableType) {
		return InputType == TableType || InputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(InputType, TableType);
	}
};

#undef LOCTEXT_NAMESPACE
