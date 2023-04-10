#include "MainPage.h"

#include "AddAccountPage.h"
#include "CloudProviderAccountModel.h"
#include "FileListPage.h"
#include "FileListPageModel.h"
#include "MainPage.g.cpp"
#include "WebViewPage.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::Foundation::Collections::IObservableVector;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Controls::NavigationView;
using ::winrt::Windows::UI::Xaml::Controls::NavigationViewItem;
using ::winrt::Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs;
using ::winrt::Windows::UI::Xaml::Controls::NavigationViewItemSeparator;
using ::winrt::Windows::UI::Xaml::Controls::Symbol;
using ::winrt::Windows::UI::Xaml::Controls::SymbolIcon;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

constexpr std::wstring_view kAddAccountPageTag = L"account";

}  // namespace

void MainPage::OnNavigatedTo(const NavigationEventArgs& e) {
  model_ = e.Parameter().as<coro_cloudbrowser_winrt::MainPageModel>();
}

void MainPage::OnNavigatedFrom(const NavigationEventArgs&) {
  model_.Accounts().VectorChanged(accounts_changed_);
}

void MainPage::NavViewLoaded(IInspectable const&, RoutedEventArgs const&) {
  accounts_changed_ = model_.Accounts().VectorChanged(
      [&](const auto& /*sender*/, const auto& /*args*/) { UpdateMenu(); });
  UpdateMenu();
}

void MainPage::MenuItemInvoked(
    const Windows::UI::Xaml::Controls::NavigationView& sender,
    const NavigationViewItemInvokedEventArgs& args) {
  if (args.IsSettingsInvoked()) {
    ContentFrame().Navigate(
        xaml_typename<coro_cloudbrowser_winrt::WebViewPage>(),
        box_value(winrt::to_hstring(CORO_CLOUDSTORAGE_REDIRECT_URI)));
    return;
  }
  for (const auto& item : sender.MenuItems()) {
    auto entry = item.try_as<NavigationViewItem>();
    if (entry && entry.Content() == args.InvokedItem()) {
      if (entry.Tag().try_as<hstring>() == kAddAccountPageTag) {
        ContentFrame().Navigate(
            xaml_typename<coro_cloudbrowser_winrt::AddAccountPage>(),
            model_.ProviderTypes());
      } else if (auto account = args.InvokedItem()
                                    .try_as<coro_cloudbrowser_winrt::
                                                CloudProviderAccountModel>()) {
        ContentFrame().Navigate(
            xaml_typename<coro_cloudbrowser_winrt::FileListPage>(),
            winrt::make<FileListPageModel>(std::move(account), /*path=*/L"/"));
      }
    }
  }
}

void MainPage::UpdateMenu() {
  NavigationView().MenuItems().Clear();

  for (const auto& account : model_.Accounts()) {
    NavigationViewItem item;
    item.ContentTemplate(NavViewMenuItemTemplate());
    item.Content(account);
    NavigationView().MenuItems().Append(std::move(item));
  }

  NavigationView().MenuItems().Append(NavigationViewItemSeparator());

  {
    NavigationViewItem item;
    item.Content(box_value(L"Add Account"));
    item.Tag(box_value(kAddAccountPageTag));
    item.Icon(SymbolIcon(Symbol::Add));
    NavigationView().MenuItems().Append(std::move(item));
  }

  auto accounts = model_.Accounts();
  if (accounts.Size() > 0) {
    ContentFrame().Navigate(
        xaml_typename<coro_cloudbrowser_winrt::FileListPage>(),
        winrt::make<FileListPageModel>(accounts.GetAt(accounts.Size() - 1),
                                       /*path=*/L"/"));
  } else {
    ContentFrame().Navigate(
        xaml_typename<coro_cloudbrowser_winrt::AddAccountPage>(),
        model_.ProviderTypes());
  }
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
