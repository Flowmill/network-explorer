// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <config/intake_config.h>

#include <channel/tcp_channel.h>
#include <util/environment_variables.h>
#include <util/log.h>

#include <util/utility.h>

#include <cstdlib>

namespace config {

FileDescriptor IntakeConfig::create_output_record_file() const
{
  FileDescriptor fd;

  LOG::debug("intake record file: `{}`", record_path_);
  if (!record_path_.empty()) {
    if (auto const error = fd.create(record_path_.c_str(), FileDescriptor::Access::write_only)) {
      LOG::error("failed to create intake record file at `{}`", record_path_);
    } else {
      LOG::debug("created intake record file at `{}`", record_path_);
    }
  }

  return fd;
}

std::unique_ptr<channel::NetworkChannel> IntakeConfig::make_channel(uv_loop_t &loop) const
{
  return std::make_unique<channel::TCPChannel>(loop, host_, port_);
}

IntakeConfig IntakeConfig::read_from_env()
{
  IntakeConfig intake{
      .host_ = get_env_var(INTAKE_HOST_VAR),
      .port_ = get_env_var(INTAKE_PORT_VAR),
      .record_output_path = std::string(try_get_env_var(INTAKE_RECORD_OUTPUT_PATH_VAR)),
      .encoder_ = try_get_env_value<IntakeEncoder>(INTAKE_INTAKE_ENCODER_VAR)};

  return intake;
}

IntakeConfig::ArgsHandler::ArgsHandler(cli::ArgsParser &parser)
    : encoder_(parser.add_arg(
          "intake-encoder",
          "Chooses the intake encoder to use"
          " - this relates to the sink used to dump collected telemetry to",
          INTAKE_INTAKE_ENCODER_VAR,
          IntakeEncoder::binary))
{}

IntakeConfig IntakeConfig::ArgsHandler::read_config()
{
  auto intake_config = config::IntakeConfig::read_from_env();

  intake_config.encoder(*encoder_);

  return intake_config;
}

} // namespace config
