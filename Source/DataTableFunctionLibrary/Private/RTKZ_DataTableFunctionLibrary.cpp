#include "RTKZ_DataTableFunctionLibrary.h"
#include "Blueprint/BlueprintSupport.h"

#define LOCTEXT_NAMESPACE "RTKZ_DataTableFunctionLibrary"

const FName TableInvalidWarning = FName("RTKZ_TableInvalidWarning");
const FName ParentTableInvalidWarning = FName("RTKZ_ParentTableInvalidWarning");
const FName RowStructsIncompatibleWarning = FName("RTKZ_RowStructsIncompatibleWarning");

static bool RowTypeCompatibleWithTable(UScriptStruct const* InputType, UScriptStruct const* TableType) {
	return InputType == TableType || InputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(InputType, TableType);
}

URTKZ_DataTableFunctionLibrary::URTKZ_DataTableFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FBlueprintSupport::RegisterBlueprintWarning(FBlueprintWarningDeclaration(TableInvalidWarning, LOCTEXT("TableInvalidWarning", "The table is invalid")));
	FBlueprintSupport::RegisterBlueprintWarning(FBlueprintWarningDeclaration(ParentTableInvalidWarning, LOCTEXT("ParentTableInvalidWarning", "The parent table is invalid")));
	FBlueprintSupport::RegisterBlueprintWarning(FBlueprintWarningDeclaration(RowStructsIncompatibleWarning, LOCTEXT("RowStructsIncompatibleWarning", "Incompatible row structs")));
}

void URTKZ_DataTableFunctionLibrary::AppendParentDataTables(UCompositeDataTable* DataTable, const TArray<UDataTable*>& NewTables)
{
	if (!DataTable) {
		FFrame::KismetExecutionMessage(TEXT("The table is invalid."), ELogVerbosity::Warning, TableInvalidWarning);
		return;
	}
	UScriptStruct const* TableType = DataTable->GetRowStruct();
	for (UDataTable* ParentTable : NewTables)
	{
		if (!ParentTable) {
			FFrame::KismetExecutionMessage(TEXT("The parent table is invalid."), ELogVerbosity::Warning, ParentTableInvalidWarning);
			return;
		}
		UScriptStruct const* ParentTableType = ParentTable->GetRowStruct();
		if (ParentTableType != TableType) {
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("The parent row struct %s is incompatible with the table row struct %s."), *GetPathNameSafe(ParentTableType), *GetPathNameSafe(TableType)), ELogVerbosity::Warning, RowStructsIncompatibleWarning);
			return;
		}
	}
	DataTable->AppendParentTables(NewTables);
}

void URTKZ_DataTableFunctionLibrary::RemoveDataTableRow(UDataTable* DataTable, FName RowName)
{
	if (!DataTable) {
		FFrame::KismetExecutionMessage(TEXT("The table is invalid."), ELogVerbosity::Warning, TableInvalidWarning);
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
