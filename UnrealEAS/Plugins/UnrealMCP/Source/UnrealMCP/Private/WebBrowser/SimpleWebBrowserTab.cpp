#include "WebBrowser/SimpleWebBrowserTab.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

void SSimpleWebBrowserTab::Construct(const FArguments& InArgs)
{
    InitialURL = InArgs._InitialURL;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(0)
        [
            SNew(SBox)
            .MinDesiredWidth(800.0f)
            .MinDesiredHeight(600.0f)
            [
                SAssignNew(WebBrowser, SWebBrowser)
                .InitialURL(InitialURL)
                .ShowControls(false)
                .SupportsTransparency(false)
            ]
        ]
    ];

    UE_LOG(LogTemp, Log, TEXT("SSimpleWebBrowserTab: Created with URL: %s"), *InitialURL);
}