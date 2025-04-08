#include "DLLoadingScreenManager.h"

#include "LoadingProcessInterface.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "ShaderPipelineCache.h"

bool ILoadingProcessInterface::ShouldShowLoadingScreen(UObject* TestObject)
{
    if (TestObject != nullptr)
    {
        if (ILoadingProcessInterface* LoadObserver = Cast<ILoadingProcessInterface>(TestObject))
        {
            if (LoadObserver->ShouldShowLoadingScreen())
            {
                return true;
            }
        }
    }
    return false;
}

class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:

    FLoadingScreenInputPreProcessor() {}
    virtual ~FLoadingScreenInputPreProcessor() {}

    bool CanEatInput() const
    {
        return !GIsEditor;
    }

    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
    virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanEatInput(); }
    virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
    virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
    virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return true; }
    virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanEatInput(); }
    virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanEatInput(); }
};

void UDLLoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 맵 로딩 전/후 델리게이트 바인딩
    FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UDLLoadingScreenManager::HandlePreLoadMap);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UDLLoadingScreenManager::HandlePostLoadMap);
}

void UDLLoadingScreenManager::Deinitialize()
{
    Super::Deinitialize();

    FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UDLLoadingScreenManager::Tick(float DeltaTime)
{
    UpdateLoadingScreenVisibility();
}

bool UDLLoadingScreenManager::IsTickable() const
{
    return !HasAnyFlags(RF_ClassDefaultObject);    
}

UWorld* UDLLoadingScreenManager::GetTickableGameObjectWorld() const
{
    return GetGameInstance()->GetWorld();
}

void UDLLoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
    if (WorldContext.OwningGameInstance == GetGameInstance())
    {
        bCurrentlyInLoadMap = true;
        if (GEngine->IsInitialized())
        {
            UpdateLoadingScreenVisibility();
        }
    }
}

void UDLLoadingScreenManager::HandlePostLoadMap(UWorld* LoadedWorld)
{
    if ((LoadedWorld != nullptr) && (LoadedWorld->GetGameInstance() == GetGameInstance()))
    {
        bCurrentlyInLoadMap = false;
    }
}

void UDLLoadingScreenManager::UpdateLoadingScreenVisibility()
{
    if (ShouldShowLoadingScreen())
    {
        ShowLoadingScreen();
        StartBlockingInput();
        ChangePerformanceSettings(true);
    }
    else
    {
        HideLoadingScreen();
        StopBlockingInput();
        ChangePerformanceSettings(false);
    }
}

void UDLLoadingScreenManager::ShowLoadingScreen()
{
    if (bCurrentlyShowingLoadingScreen) return;
    bCurrentlyShowingLoadingScreen = true;

    TSubclassOf<UUserWidget> WidgetClass = LoadingScreenWidgetClass.TryLoadClass<UUserWidget>();
    if (WidgetClass)
    {
        UGameInstance* LocalGameInstance = GetGameInstance();
        if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*LocalGameInstance, WidgetClass, NAME_None))
        {
            TSharedRef<SWidget> SlateWidget = UserWidget->TakeWidget();
            ActiveLoadingWidget = SlateWidget;

            if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
            {
                GameViewportClient->AddViewportWidgetContent(SlateWidget, 1000);               
            }
            if (!GIsEditor)
            {
                FSlateApplication::Get().Tick();
            }
        }
    }    
}

void UDLLoadingScreenManager::HideLoadingScreen()
{
    if (!bCurrentlyShowingLoadingScreen)
    {
        return;
    }
    
    if (ActiveLoadingWidget.IsValid())
    {
        if (UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
        {
            GameViewportClient->RemoveViewportWidgetContent(ActiveLoadingWidget.ToSharedRef());
        }
        ActiveLoadingWidget.Reset();  
    }
    bCurrentlyShowingLoadingScreen = false;
    GEngine->ForceGarbageCollection(true);
}

bool UDLLoadingScreenManager::ShouldShowLoadingScreen()
{
    const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();
    bool bWantToForceShowLoadingScreen = false;

    if (bNeedToShowLoadingScreen)
    {
        TimeLoadingScreenLastDismissed = -1.0;
    }
    else 
    {
        const double CurrentTime = FPlatformTime::Seconds();
        if (TimeLoadingScreenLastDismissed < 0.0)
        {
            TimeLoadingScreenLastDismissed = CurrentTime;
        }

        const double TimeSinceScreenDismissed = CurrentTime - TimeLoadingScreenLastDismissed;
        if (TimeSinceScreenDismissed < HoldLoadingScreenDuration)
        {
            bWantToForceShowLoadingScreen = true;
        }
    }
    return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool UDLLoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
    if (bCurrentlyInLoadMap) return true;

    auto LocalGameInstance = GetGameInstance();
    if (!LocalGameInstance) return true;

    for (UGameInstanceSubsystem* Subsystem : LocalGameInstance->GetSubsystemArrayCopy<UGameInstanceSubsystem>())
    {
        if (ILoadingProcessInterface::ShouldShowLoadingScreen(Subsystem))
        {
            return true;
        }
    }

    return false;
}

void UDLLoadingScreenManager::StartBlockingInput()
{
    if (!InputPreProcessor.IsValid())
    {
        InputPreProcessor = MakeShared<FLoadingScreenInputPreProcessor>();
        FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
    }
}

void UDLLoadingScreenManager::StopBlockingInput()
{
    if (InputPreProcessor.IsValid())
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
        InputPreProcessor.Reset();
    }
}

void UDLLoadingScreenManager::ChangePerformanceSettings(bool bEnableLoadingScreen)
{
    if (UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
    {
        // 월드 렌더링 차단
        GameViewportClient->bDisableWorldRendering = bEnableLoadingScreen;
    }

    // ShaderPipelineCache는 로딩 시 최적화를 위해 빠른 모드로 전환
    FShaderPipelineCache::SetBatchMode(bEnableLoadingScreen ?
        FShaderPipelineCache::BatchMode::Fast :
        FShaderPipelineCache::BatchMode::Background);
}