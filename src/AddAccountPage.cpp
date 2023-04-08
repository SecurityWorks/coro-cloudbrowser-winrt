#include "AddAccountPage.h"

#if __has_include("AddAccountPage.g.cpp")
#include "AddAccountPage.g.cpp"
#endif

#include "CloudProviderTypeModel.h"
#include "WebViewPage.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::Foundation::Collections::IVector;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Controls::ItemClickEventArgs;
using ::winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

void AddAccountPage::OnNavigatedTo(const NavigationEventArgs& args) {
  ItemListView().ItemsSource(
      args.Parameter()
          .as<IVector<coro_cloudbrowser_winrt::CloudProviderTypeModel>>());
}

winrt::fire_and_forget AddAccountPage::ItemListViewItemClick(
    const IInspectable&, const ItemClickEventArgs& e) {
  auto type =
      e.ClickedItem().as<coro_cloudbrowser_winrt::CloudProviderTypeModel>();
  hstring authorization_url = type.AuthorizationUrl();
  if (std::wstring_view(authorization_url).find(L"cloudbrowser.oauth") !=
      std::wstring_view::npos) {
    co_await Windows::System::Launcher::LaunchUriAsync(
        Windows::Foundation::Uri(authorization_url));
  } else {
    Frame().Navigate(xaml_typename<coro_cloudbrowser_winrt::WebViewPage>(),
                     winrt::box_value(authorization_url));
  }
}

void AddAccountPage::ItemListViewKeyDown(const IInspectable&,
                                         const KeyRoutedEventArgs&) {}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
