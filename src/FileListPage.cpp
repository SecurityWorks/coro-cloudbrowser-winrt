#include "FileListPage.h"
#if __has_include("FileListPage.g.cpp")
#include "FileListPage.g.cpp"
#endif

#include <coro/cloudstorage/util/cloud_provider_utils.h>
#include <coro/cloudstorage/util/string_utils.h>
#include <coro/http/http_parse.h>
#include <coro/util/raii_utils.h>
#include <fmt/format.h>

#include "FileListEntryModel.h"
#include "FileListPageModel.h"
#include "FilePreviewPage.h"
#include "FilePreviewPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::AbstractCloudProvider;
using ::coro::cloudstorage::util::StrCat;
using ::coro::http::DecodeUri;
using ::coro::http::EncodeUri;
using ::coro::util::AtScopeExit;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::IAsyncOperation;
using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::UI::Xaml::DependencyProperty;
using ::winrt::Windows::UI::Xaml::ExceptionRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::UIElement;
using ::winrt::Windows::UI::Xaml::Visibility;
using ::winrt::Windows::UI::Xaml::Controls::Image;
using ::winrt::Windows::UI::Xaml::Controls::ItemClickEventArgs;
using ::winrt::Windows::UI::Xaml::Controls::ListViewPersistenceHelper;
using ::winrt::Windows::UI::Xaml::Input::GettingFocusEventArgs;
using ::winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Media::Animation::Storyboard;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

}  // namespace

IAsyncAction FileListPage::OnNavigatedTo(NavigationEventArgs e) {
  page_model_ = e.Parameter().as<coro_cloudbrowser_winrt::FileListPageModel>();
  FileList().ItemsSource(page_model_.Items());
  Path().Text(
      winrt::to_hstring(DecodeUri(winrt::to_string(page_model_.Path()))));

  if (!page_model_.ScrollPosition().empty()) {
    co_await ListViewPersistenceHelper::SetRelativeScrollPositionAsync(
        FileList(), page_model_.ScrollPosition(),
        [current_items = page_model_.Items()](
            const hstring& key) -> IAsyncOperation<IInspectable> {
          for (const auto& item : current_items) {
            if (item.Id() == key) {
              co_return item;
            }
          }
          co_return nullptr;
        });
  }

  co_await RefreshContent();
}

void FileListPage::OnNavigatedFrom(const NavigationEventArgs&) {
  page_model_.ScrollPosition(
      ListViewPersistenceHelper::GetRelativeScrollPosition(
          FileList(), [](const IInspectable& d) {
            return d.as<FileListEntryModel>()->Id();
          }));
}

void FileListPage::OnGettingFocus(const UIElement&,
                                  const GettingFocusEventArgs&) {}

void FileListPage::OnKeyDown(const IInspectable&, const KeyRoutedEventArgs&) {}

void FileListPage::OnKeyDown(const KeyRoutedEventArgs&) {}

void FileListPage::FileListEntryClick(const IInspectable&,
                                      const ItemClickEventArgs& e) {
  auto entry = e.ClickedItem().as<FileListEntryModel>();
  if (entry->Type() == FileType::kDirectory) {
    winrt::hstring path = winrt::to_hstring(
        StrCat(winrt::to_string(page_model_.Path()),
               EncodeUri(winrt::to_string(entry->Filename())), '/'));
    Frame().Navigate(
        xaml_typename<coro_cloudbrowser_winrt::FileListPage>(),
        winrt::make<FileListPageModel>(page_model_.Account(), std::move(path)));
  } else if (entry->Type() == FileType::kAudio ||
             entry->Type() == FileType::kVideo) {
    Frame().Navigate(
        xaml_typename<coro_cloudbrowser_winrt::FilePreviewPage>(),
        winrt::make<FilePreviewPageModel>(entry->Filename(), entry->Uri()));
  }
}

void FileListPage::FileListEntryCheckboxChecked(const IInspectable&,
                                                const RoutedEventArgs&) {}

void FileListPage::FileListEntryCheckboxUnchecked(const IInspectable&,
                                                  const RoutedEventArgs&) {}

void FileListPage::FileListEntryThumbnailImageOpened(const IInspectable& sender,
                                                     const RoutedEventArgs&) {
  auto image = sender.as<Image>();
  auto data = image.DataContext().as<FileListEntryModel>();
  if (data) {
    data->ThumbnailVisibility(Visibility::Visible);
    data->IconVisibility(Visibility::Collapsed);
    image.Resources()
        .Lookup(winrt::box_value(L"Animation"))
        .as<Storyboard>()
        .Begin();
  }
}

void FileListPage::FileListEntryThumbnailImageFailed(
    const IInspectable&, const ExceptionRoutedEventArgs& args) {
  std::stringstream sstream;
  sstream << "Failed to load thumbnail: " << to_string(args.ErrorMessage())
          << '\n';
  OutputDebugStringA(sstream.str().c_str());
}

void FileListPage::FileListEntryThumbnailImageLoaded(const IInspectable& sender,
                                                     const RoutedEventArgs&) {
  sender.as<Image>().RegisterPropertyChangedCallback(
      Image::SourceProperty(),
      [](const IInspectable& sender, const DependencyProperty&) {
        auto data = sender.as<Image>().DataContext().as<FileListEntryModel>();
        if (data) {
          data->ThumbnailVisibility(Visibility::Collapsed);
          data->IconVisibility(Visibility::Visible);
        }
      });
}

void FileListPage::BackButtonClick(const IInspectable&,
                                   const RoutedEventArgs&) {
  if (Frame().CanGoBack()) {
    Frame().GoBack();
  }
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

IAsyncAction FileListPage::RefreshContent() {
  ProgressBar().Visibility(Visibility::Visible);
  auto at_exit = AtScopeExit([progress_bar = ProgressBar()] {
    progress_bar.Visibility(Visibility::Collapsed);
  });

  auto path = page_model_.Path();
  auto account = page_model_.Account().as<CloudProviderAccountModel>();
  auto current_items = page_model_.Items();
  bool empty = current_items.Size() == 0;

  concurrency::cancellation_token_source stop_source;
  auto stop_token = co_await winrt::get_cancellation_token();
  stop_token.callback([stop_source] { stop_source.cancel(); });

  std::vector<coro_cloudbrowser_winrt::FileListEntryModel> updated;
  try {
    auto directory = co_await account->GetItemByPath(winrt::to_string(path),
                                                     stop_source.get_token());
    std::optional<std::string> page_token;
    do {
      auto page = co_await account->ListDirectoryPage(
          std::get<AbstractCloudProvider::Directory>(directory), page_token,
          stop_source.get_token());
      for (auto item : page.items) {
        auto entry = winrt::make<FileListEntryModel>(
            account->Id(), to_string(path), std::move(item));
        if (empty) {
          current_items.Append(std::move(entry));
        } else {
          updated.emplace_back(std::move(entry));
        }
      }
      page_token = std::move(page.next_page_token);
    } while (page_token);
  } catch (const coro::Exception& e) {
    std::stringstream stream;
    stream << e.what() << '\n';
    stream << winrt::to_string(path) << '\n';
    OutputDebugStringA(stream.str().c_str());
  }

  if (empty) {
    co_return;
  }

  std::unordered_map<hstring, uint32_t> previous;
  for (uint32_t i = 0; i < current_items.Size(); i++) {
    previous.emplace(current_items.GetAt(i).Id(), i);
  }

  std::unordered_set<hstring> current;
  for (auto entry : updated) {
    current.insert(entry.Id());
    auto it = previous.find(entry.Id());
    if (it == previous.end()) {
      current_items.InsertAt(0, std::move(entry));
    } else {
      current_items.GetAt(it->second).as<FileListEntryModel>()->Update(entry);
    }
  }

  for (uint32_t i = 0; i < current_items.Size();) {
    auto it = current.find(current_items.GetAt(i).Id());
    if (it == current.end()) {
      current_items.RemoveAt(i);
    } else {
      i++;
    }
  }
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
