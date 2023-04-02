#include "FileListPage.h"
#if __has_include("FileListPage.g.cpp")
#include "FileListPage.g.cpp"
#endif

#include <coro/cloudstorage/util/string_utils.h>
#include <coro/http/http_parse.h>
#include <fmt/format.h>

#include "FileListEntryModel.h"
#include "FileListPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::StrCat;
using ::coro::http::EncodeUri;
using ::coro::http::EncodeUriPath;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::UIElement;
using ::winrt::Windows::UI::Xaml::Controls::ItemClickEventArgs;
using ::winrt::Windows::UI::Xaml::Input::GettingFocusEventArgs;
using ::winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

std::string GetThumbnailUri(
    const coro::cloudstorage::util::CloudProviderAccount::Id& id,
    std::string_view path,
    const coro::cloudstorage::util::AbstractCloudProvider::Item& item) {
  return fmt::format(
      "http://localhost:12345/list/{}/{}{}?thumbnail=true", id.type,
      EncodeUri(id.username),
      EncodeUriPath(StrCat(
          path, std::visit([](const auto& d) { return d.name; }, item))));
}

}  // namespace

IAsyncAction FileListPage::OnNavigatedTo(NavigationEventArgs e) {
  auto current_items = winrt::single_threaded_observable_vector<
      coro_cloudbrowser_winrt::FileListEntryModel>();
  FileList().ItemsSource(current_items);

  auto model = e.Parameter().as<coro_cloudbrowser_winrt::FileListPageModel>();
  auto account = model.Account().as<CloudProviderAccountModel>();

  concurrency::cancellation_token_source stop_source;
  auto stop_token = co_await winrt::get_cancellation_token();
  stop_token.callback([stop_source] { stop_source.cancel(); });

  try {
    auto root = co_await account->GetRoot(stop_source.get_token());
    auto page = co_await account->ListDirectoryPage(
        root, /*page_token=*/std::nullopt, stop_source.get_token());

    for (auto item : page.items) {
      auto thumbnail_uri =
          GetThumbnailUri(account->Id(), to_string(model.Path()), item);
      current_items.Append(winrt::make<FileListEntryModel>(
          winrt::to_hstring(thumbnail_uri), std::move(item)));
    }

    std::stringstream stream;
    stream << page.items.size() << '\n';
    OutputDebugStringA(stream.str().c_str());
  } catch (const coro::Exception& e) {
    OutputDebugStringA(e.what());
  }

  co_return;
}

void FileListPage::OnGettingFocus(const UIElement&,
                                  const GettingFocusEventArgs&) {}

void FileListPage::OnKeyDown(const IInspectable&, const KeyRoutedEventArgs&) {}

void FileListPage::OnKeyDown(const KeyRoutedEventArgs&) {}

void FileListPage::FileListEntryClick(const IInspectable&,
                                      const ItemClickEventArgs&) {}

void FileListPage::FileListEntryCheckboxChecked(const IInspectable&,
                                                const RoutedEventArgs&) {}

void FileListPage::FileListEntryCheckboxUnchecked(const IInspectable&,
                                                  const RoutedEventArgs&) {}

void FileListPage::FileListEntryThumbnailLoaded(const IInspectable&,
                                                const RoutedEventArgs&) {}

void FileListPage::BackButtonClick(const IInspectable&,
                                   const RoutedEventArgs&) {}

void FileListPage::CreateDirectoryClick(const IInspectable&,
                                        const RoutedEventArgs&) {}

void FileListPage::UploadClick(const IInspectable&, const RoutedEventArgs&) {}

void FileListPage::DeleteClick(const IInspectable&, const RoutedEventArgs&) {}

void FileListPage::RenameClick(const IInspectable&, const RoutedEventArgs&) {}

void FileListPage::MoveClick(const IInspectable&, const RoutedEventArgs&) {}

void FileListPage::MoveCancelClick(const IInspectable&,
                                   const RoutedEventArgs&) {}

void FileListPage::DownloadClick(const IInspectable&, const RoutedEventArgs&) {}

void FileListPage::RefreshClick(const IInspectable&, const RoutedEventArgs&) {}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
