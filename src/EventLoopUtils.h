#pragma once

#include <coro/task.h>
#include <coro/util/event_loop.h>

namespace coro::cloudbrowser::util {

Task<> SwitchTo(coro::util::EventLoop*) noexcept;

}  // namespace coro::cloudbrowser::util