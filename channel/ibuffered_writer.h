/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <platform/platform.h>
#include <util/expected.h>

#include <system_error>

#include <cstring>

// Interface for classes that write through a buffer.
//
class IBufferedWriter {
public:
  virtual ~IBufferedWriter() {}

  // Starts a new write.
  //
  // Returns the memory where caller should write the data, or nullptr in case
  // of an error.
  //
  virtual Expected<u8 *, std::error_code> start_write(u32 length) = 0;

  // Finishes the current write.
  //
  virtual void finish_write() = 0;

  // Writes the given payload in smaller batches to fit the internal buffer
  Expected<bool, std::error_code> write_as_chunks(std::string_view payload)
  {
    for (auto const max = buf_size(); !payload.empty();) {
      auto const size = (max <= payload.size()) ? max : static_cast<u32>(payload.size());

      auto const allocated = start_write(size);
      if (!allocated) {
        return {unexpected, allocated.error()};
      }

      memcpy(*allocated, payload.data(), size);
      finish_write();
      payload.remove_prefix(size);
    }

    return true;
  }

  // Flushes the buffer.
  //
  virtual std::error_code flush() = 0;

  // Returns the buffer size.
  //
  virtual u32 buf_size() const = 0;

  virtual bool is_writable() const = 0;
};
