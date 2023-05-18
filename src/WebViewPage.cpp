#include "WebViewPage.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif

#include "WebViewPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

void WebViewPage::OnNavigatedTo(const NavigationEventArgs& args) {
  const auto& page_model = args.Parameter().as<WebViewPageModel>();
  page_model->OnNavigatedTo()();
  WebView().Source(page_model->SourceUri());
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
