#include "FileListPage.h"
#if __has_include("FileListPage.g.cpp")
#include "FileListPage.g.cpp"
#endif

#include <coro/cloudstorage/util/cloud_provider_utils.h>
#include <coro/cloudstorage/util/string_utils.h>
#include <coro/http/http_parse.h>
#include <fmt/format.h>

#include "FileListEntryModel.h"
#include "FileListPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::AbstractCloudProvider;
using ::coro::cloudstorage::util::CloudProviderAccount;
using ::coro::cloudstorage::util::StrCat;
using ::coro::http::EncodeUri;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::UIElement;
using ::winrt::Windows::UI::Xaml::Controls::ItemClickEventArgs;
using ::winrt::Windows::UI::Xaml::Input::GettingFocusEventArgs;
using ::winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

std::string GetThumbnailUri(const CloudProviderAccount::Id& id,
                            std::string_view path,
                            const AbstractCloudProvider::Item& item) {
  return fmt::format(
      "http://localhost:12345/list/{}/{}{}?thumbnail=true", id.type,
      EncodeUri(id.username),
      StrCat(path, std::visit([](const auto& d) { return EncodeUri(d.name); },
                              item)));
}

}  // namespace

IAsyncAction FileListPage::OnNavigatedTo(NavigationEventArgs e) {
  auto current_items = winrt::single_threaded_observable_vector<
      coro_cloudbrowser_winrt::FileListEntryModel>();
  FileList().ItemsSource(current_items);

  page_model_ = e.Parameter().as<coro_cloudbrowser_winrt::FileListPageModel>();
  auto account = page_model_->Account().as<CloudProviderAccountModel>();

  concurrency::cancellation_token_source stop_source;
  auto stop_token = co_await winrt::get_cancellation_token();
  stop_token.callback([stop_source] { stop_source.cancel(); });

  try {
    auto directory = co_await account->GetItemByPath(
        winrt::to_string(page_model_->Path()), stop_source.get_token());
    auto page = co_await account->ListDirectoryPage(
        std::get<AbstractCloudProvider::Directory>(directory),
        /*page_token=*/std::nullopt, stop_source.get_token());

    for (auto item : page.items) {
      auto thumbnail_uri =
          GetThumbnailUri(account->Id(), to_string(page_model_->Path()), item);
      current_items.Append(winrt::make<FileListEntryModel>(
          winrt::to_hstring(thumbnail_uri), std::move(item)));
    }
  } catch (const coro::Exception& e) {
    std::stringstream stream;
    stream << e.what() << '\n';
    stream << winrt::to_string(page_model_->Path()) << '\n';
    OutputDebugStringA(stream.str().c_str());
  }

  co_return;
}

void FileListPage::OnGettingFocus(const UIElement&,
                                  const GettingFocusEventArgs&) {}

void FileListPage::OnKeyDown(const IInspectable&, const KeyRoutedEventArgs&) {}

void FileListPage::OnKeyDown(const KeyRoutedEventArgs&) {}

void FileListPage::FileListEntryClick(const IInspectable& sender,
                                      const ItemClickEventArgs& e) {
  auto entry = e.ClickedItem().as<FileListEntryModel>();
  winrt::hstring path = winrt::to_hstring(
      StrCat(winrt::to_string(page_model_->Path()),
             EncodeUri(winrt::to_string(entry->Filename())), '/'));
  page_model_->OnItemClick()(sender, path);
}

void FileListPage::FileListEntryCheckboxChecked(const IInspectable&,
                                                const RoutedEventArgs&) {}

void FileListPage::FileListEntryCheckboxUnchecked(const IInspectable&,
                                                  const RoutedEventArgs&) {}

void FileListPage::FileListEntryThumbnailLoaded(const IInspectable&,
                                                const RoutedEventArgs&) {}

void FileListPage::BackButtonClick(const IInspectable& sender,
                                   const RoutedEventArgs& args) {
  page_model_->OnBackClick()(sender, args);
}

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
