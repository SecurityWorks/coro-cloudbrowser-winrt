﻿#include "MainPage.h"

#include "AddAccountPage.h"
#include "CloudProviderAccountModel.h"
#include "MainPage.g.cpp"

namespace winrt::coro_cloudbrowser_winrt::implementation {

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

void MainPage::OnNavigatedTo(NavigationEventArgs e) {
  accounts_ = e.Parameter()
                  .as<IObservableVector<
                      coro_cloudbrowser_winrt::CloudProviderAccountModel>>();
}

void MainPage::OnNavigatedFrom(NavigationEventArgs e) {
  accounts_.VectorChanged(accounts_changed_);
}

void MainPage::NavViewLoaded(IInspectable const&, RoutedEventArgs const&) {
  accounts_changed_ = accounts_.VectorChanged(
      [&](const auto& /*sender*/, const auto& /*args*/) { UpdateMenu(); });
  UpdateMenu();
}

void MainPage::MenuItemInvoked(
    const Windows::UI::Xaml::Controls::NavigationView& sender,
    const NavigationViewItemInvokedEventArgs& args) {
  for (const auto& item : sender.MenuItems()) {
    auto entry = item.try_as<NavigationViewItem>();
    if (entry && entry.Content() == args.InvokedItem()) {
      if (entry.Tag().try_as<hstring>() == kAddAccountPageTag) {
        ContentFrame().Navigate(
            xaml_typename<coro_cloudbrowser_winrt::AddAccountPage>());
      }
    }
  }
}

void MainPage::UpdateMenu() {
  NavigationView().MenuItems().Clear();

  for (const auto& account : accounts_) {
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
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
