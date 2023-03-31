#pragma once

#include "FileListPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListPage : FileListPageT<FileListPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FileListPage
    : FileListPageT<FileListPage, implementation::FileListPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation
