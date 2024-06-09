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

#undef LOCTEXT_NAMESPACE
