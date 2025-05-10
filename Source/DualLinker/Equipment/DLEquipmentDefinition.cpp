// Fill out your copyright notice in the Description page of Project Settings.


#include "DLEquipmentDefinition.h"
#include "DLEquipmentInstance.h"

UDLEquipmentDefinition::UDLEquipmentDefinition()
{
	InstanceType = UDLEquipmentInstance::StaticClass();
}
