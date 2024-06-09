#include "RTKZ_DataTableFunctionLibrary.h"
#include "Misc/RuntimeErrors.h"

#define LOCTEXT_NAMESPACE "RTKZ_DataTableFunctionLibrary"

URTKZ_DataTableFunctionLibrary::URTKZ_DataTableFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void URTKZ_DataTableFunctionLibrary::AppendParentDataTables(UCompositeDataTable* DataTable, const TArray<UDataTable*>& NewTables)
{
	if (!DataTable) {
		LogRuntimeError(LOCTEXT("AppendParentDataTables_InvalidTable", "AppendParentDataTables: The table is invalid."));
		return;
	}
	UScriptStruct const* TableType = DataTable->GetRowStruct();
	for (UDataTable* ParentTable : NewTables)
	{
		if (!ParentTable) {
			LogRuntimeError(LOCTEXT("AppendParentDataTables_InvalidParent", "AppendParentDataTables: The parent table is invalid."));
			return;
		}
		UScriptStruct const* ParentTableType = ParentTable->GetRowStruct();
		if (ParentTableType != TableType) {
			FFormatNamedArguments Args;
			Args.Add(TEXT("ParentTableType"), FText::AsCultureInvariant(GetPathNameSafe(ParentTableType)));
			Args.Add(TEXT("TableType"), FText::AsCultureInvariant(GetPathNameSafe(TableType)));
			LogRuntimeError(FText::Format(LOCTEXT("AppendParentDataTables_Incompatible", "AppendParentDataTables: The parent row struct {ParentTableType} is incompatible with the table row struct {TableType}."), Args));
			return;
		}
	}
	DataTable->AppendParentTables(NewTables);
}

void URTKZ_DataTableFunctionLibrary::RemoveDataTableRow(UDataTable* DataTable, FName RowName)
{
	if (!DataTable) {
		LogRuntimeError(LOCTEXT("RemoveDataTableRow_InvalidTable", "RemoveDataTableRow: The table is invalid."));
		return;
	}
	DataTable->RemoveRow(RowName);
}

void URTKZ_DataTableFunctionLibrary::AddDataTableRow(UDataTable* DataTable, FName RowName, const FTableRowBase& RowData)
{
	// CustomThunk, so we never reach this
	check(0);
}

DEFINE_FUNCTION(URTKZ_DataTableFunctionLibrary::execAddDataTableRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	P_GET_PROPERTY(FNameProperty, RowName);

	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* RowProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_FINISH;

	if (!DataTable)
	{
		FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, LOCTEXT("AddDataTableRow_InvalidTable", "The table is invalid."));
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}
	if (!RowProp || !RowPtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, LOCTEXT("AddDataTableRow_MissingInputProperty", "The row struct is invalid."));
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
		FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, FText::Format(LOCTEXT("AddDataTableRow_Incompatible", "The row struct {InputType} is incompatible with the table row struct {TableType}."), Args));
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		return;
	}
	P_NATIVE_BEGIN;
	DataTable->AddRow(RowName, *static_cast<FTableRowBase*>(RowPtr));
	P_NATIVE_END;
}

#undef LOCTEXT_NAMESPACE
