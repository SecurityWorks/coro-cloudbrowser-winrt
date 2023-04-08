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

void AddAccountPage::ItemListViewItemClick(const IInspectable&,
                                           const ItemClickEventArgs& e) {
  auto type =
      e.ClickedItem().as<coro_cloudbrowser_winrt::CloudProviderTypeModel>();
  Frame().Navigate(xaml_typename<coro_cloudbrowser_winrt::WebViewPage>(),
                   winrt::box_value(type.AuthorizationUrl()));
}

void AddAccountPage::ItemListViewKeyDown(const IInspectable&,
                                         const KeyRoutedEventArgs&) {}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
