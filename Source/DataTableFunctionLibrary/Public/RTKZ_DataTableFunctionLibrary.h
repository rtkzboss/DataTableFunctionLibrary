#pragma once
#include "CoreMinimal.h"
#include "Engine/CompositeDataTable.h"
#include "RTKZ_DataTableFunctionLibrary.generated.h"

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
	DECLARE_FUNCTION(execAddDataTableRow);
};
