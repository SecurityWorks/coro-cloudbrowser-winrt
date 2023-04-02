#include "EventLoopUtils.h"

#include <coro/promise.h>

namespace coro::cloudbrowser::util {

Task<> SwitchTo(coro::util::EventLoop* event_loop) noexcept {
  coro::Promise<void> promise;
  event_loop->RunOnEventLoop([&promise] { promise.SetValue(); });
  co_await promise;
}

}  // namespace coro::cloudbrowser::util