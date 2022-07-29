// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <channel/upstream_connection.h>

#include <channel/component.h>
#include <util/log.h>

namespace channel {

UpstreamConnection::UpstreamConnection(
    std::size_t buffer_size, bool allow_compression, NetworkChannel &primary_channel, Channel *secondary_channel)
    : primary_channel_(primary_channel),
      lz4_channel_(primary_channel_, buffer_size),
      allow_compression_(allow_compression),
      double_write_channel_(lz4_channel_, secondary_channel ? *secondary_channel : lz4_channel_),
      buffered_writer_(secondary_channel ? static_cast<Channel &>(double_write_channel_) : lz4_channel_, buffer_size)
{}

void UpstreamConnection::connect(Callbacks &callbacks)
{
  buffered_writer_.reset();
  primary_channel_.connect(callbacks);
}

std::error_code UpstreamConnection::send(const u8 *data, int data_len)
{
  auto buffer = buffered_writer_.start_write(data_len);
  if (!buffer) {
    return buffer.error();
  }
  memcpy(*buffer, data, data_len);
  buffered_writer_.finish_write();
  return {};
}

std::error_code UpstreamConnection::flush()
{
  return buffered_writer_.flush();
}

void UpstreamConnection::close()
{
  buffered_writer_.reset();
  primary_channel_.close();
}

void UpstreamConnection::set_compression(bool enabled)
{
  buffered_writer_.flush();

  LOG::trace_in(
      Component::upstream,
      "UpstreamConnection: {} ({}allowed) LZ4 compression",
      enabled ? "enabling" : "disabling",
      allow_compression_ ? "" : "not ");

  lz4_channel_.set_compression(enabled && allow_compression_);
}

BufferedWriter &UpstreamConnection::buffered_writer()
{
  return buffered_writer_;
}

in_addr_t const *UpstreamConnection::connected_address() const
{
  return primary_channel_.connected_address();
}

} // namespace channel
