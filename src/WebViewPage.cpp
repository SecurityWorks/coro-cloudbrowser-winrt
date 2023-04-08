#include "WebViewPage.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif

namespace winrt::coro_cloudbrowser_winrt::implementation {

using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

void WebViewPage::OnNavigatedTo(
    const Windows::UI::Xaml::Navigation::NavigationEventArgs& args) {
  WebView().Source(
      Windows::Foundation::Uri(winrt::unbox_value<hstring>(args.Parameter())));
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
