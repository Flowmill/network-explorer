/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>

#include "resync_queue_interface.h"
#include "util/element_queue_cpp.h"

namespace collector {
class ResyncQueue : public ResyncQueueProducerInterface, public ResyncQueueConsumerInterface, public ResyncChannelFactory {
public:
  ResyncQueue();
  ~ResyncQueue() override;

  // ResyncQueueConsumer
  u64 consumer_get_last_resync() const override;
  ElementQueue *consumer_get_queue() override;
  void consumer_reset() override;

  // ResyncQueueProducerInterface
  std::error_code producer_send(const u8 *data, int data_len, const u64 resync) override;
  void producer_unregister(ResyncChannel *channel) override;

  // ResyncChannelFactoryInterface
  std::unique_ptr<ResyncChannel> new_channel(std::function<void(void)> &reset_callback) override;

private:
  static constexpr u32 queue_num_elements_ = 16 * 1024;
  static constexpr u32 queue_buffer_size_ = 16 * 1024 * 1024;

  std::shared_ptr<MemElementQueueStorage> element_queue_storage_;
  ElementQueue read_queue_;
  ElementQueue write_queue_;

  mutable std::recursive_mutex mutex_;

  u64 last_resync_;
  std::unordered_set<ResyncChannel *> channels_;
}; // class ResyncQUeue
} // namespace collector
