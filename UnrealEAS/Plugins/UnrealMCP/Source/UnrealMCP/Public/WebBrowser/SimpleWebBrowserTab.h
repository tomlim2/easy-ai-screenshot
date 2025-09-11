#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Docking/SDockTab.h"

class SWebBrowser;

/**
 * Simple Web Browser Tab - minimal implementation
 */
class UNREALMCP_API SSimpleWebBrowserTab : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSimpleWebBrowserTab)
        : _InitialURL(TEXT("http://localhost:3000"))
    {}
        SLATE_ARGUMENT(FString, InitialURL)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<SWebBrowser> WebBrowser;
    FString InitialURL;
};