#pragma once

#include "DLItemData.generated.h"

class UDLItemTemplate;

UCLASS(BlueprintType, Const, meta=(DisplayName="DL Item Data"))
class UDLItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UDLItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	const UDLItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<UDLItemTemplate> ItemTemplateClass) const;
	
	void GetAllItemTemplateClasses(TArray<TSubclassOf<UDLItemTemplate>>& OutItemTemplateClasses) const;
	const TArray<TSubclassOf<UDLItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UDLItemTemplate>> ItemTemplateIDToClass;

	UPROPERTY()
	TMap<TSubclassOf<UDLItemTemplate>, int32> ItemTemplateClassToID;

private:
	UPROPERTY()
	TArray<TSubclassOf<UDLItemTemplate>> WeaponItemTemplateClasses;

};
