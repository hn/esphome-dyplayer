#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"

const size_t DYPLAYER_READ_BUFFER_LENGTH = 25;  // two messages + some extra

namespace esphome {
namespace dyplayer {

enum EqPreset {
  NORMAL = 0,
  POP = 1,
  ROCK = 2,
  JAZZ = 3,
  CLASSIC = 4,
};

enum PlayMode {
  REPEAT = 0,
  REPEATONE = 1,
  ONEOFF = 2,
  RANDOM = 3,
  REPEATDIR = 4,
  RANDOMDIR = 5,
  SEQUENCEDIR = 6,
  SEQUENCE = 7,
};

enum Device {
  USB = 0,
  SD_CARD = 1,
  FLASH = 2,
};

// See the datasheet here:
// https://grobotronics.com/images/companies/1/datasheets/DY-SV5W%20Voice%20Playback%20ModuleDatasheet.pdf
// and https://github.com/SnijderC/dyplayer
class DYPlayer : public uart::UARTDevice, public Component {
 public:
  void loop() override;

  void next();
  void previous();
  void interlude_file(uint16_t file);
  void play_file(uint16_t file);
  void select_file(uint16_t file);
  void play_folder(uint16_t folder, uint16_t file);
  void volume_up();
  void volume_down();
  void set_device(Device device);
  void set_volume(uint8_t volume);
  void set_eq(EqPreset preset);
  void set_mode(PlayMode mode);
  void sleep();
  void reset();
  void start();
  void pause();
  void stop();
  void stop_interlude();

  bool is_playing() { return is_playing_; }
  void dump_config() override;

  void add_on_finished_playback_callback(std::function<void()> callback) {
    this->on_finished_playback_callback_.add(std::move(callback));
  }

 protected:
  void send_cmd_(uint8_t cmd, uint8_t *data = {}, uint8_t len = 0);
  void send_cmd_(uint8_t cmd, uint8_t arg1) {
    uint8_t buffer[1]{arg1};
    this->send_cmd_(cmd, buffer, 1);
  }
  void send_cmd_(uint8_t cmd, uint16_t arg1) {
    uint8_t buffer[2]{(uint8_t) (arg1 >> 8), (uint8_t) arg1};
    this->send_cmd_(cmd, buffer, 2);
  }
  void send_cmd_(uint8_t cmd, uint8_t arg1, uint8_t arg2) {
    uint8_t buffer[2]{arg1, arg2};
    this->send_cmd_(cmd, buffer, 2);
  }
  uint8_t sent_cmd_{0};

  char read_buffer_[DYPLAYER_READ_BUFFER_LENGTH];
  size_t read_pos_{0};
  size_t in_len_{0};

  bool is_playing_{false};
  bool ack_set_is_playing_{false};
  bool ack_reset_is_playing_{false};

  CallbackManager<void()> on_finished_playback_callback_;
};

#define DYPLAYER_SIMPLE_ACTION(ACTION_CLASS, ACTION_METHOD) \
  template<typename... Ts> \
  class ACTION_CLASS : /* NOLINT */ \
                       public Action<Ts...>, \
                       public Parented<DYPlayer> { \
    void play(Ts... x) override { this->parent_->ACTION_METHOD(); } \
  };

DYPLAYER_SIMPLE_ACTION(NextAction, next)
DYPLAYER_SIMPLE_ACTION(PreviousAction, previous)

template<typename... Ts> class InterludeFileAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(uint16_t, file)

  void play(Ts... x) override {
    auto file = this->file_.value(x...);
    this->parent_->interlude_file(file);
  }
};

template<typename... Ts> class PlayFileAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(uint16_t, file)

  void play(Ts... x) override {
    auto file = this->file_.value(x...);
    this->parent_->play_file(file);
  }
};

template<typename... Ts> class SelectFileAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(uint16_t, file)

  void play(Ts... x) override {
    auto file = this->file_.value(x...);
    this->parent_->select_file(file);
  }
};

template<typename... Ts> class PlayFolderAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(uint16_t, folder)
  TEMPLATABLE_VALUE(uint16_t, file)

  void play(Ts... x) override {
    auto folder = this->folder_.value(x...);
    auto file = this->file_.value(x...);
    this->parent_->play_folder(folder, file);
  }
};

template<typename... Ts> class SetDeviceAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(Device, device)

  void play(Ts... x) override {
    auto device = this->device_.value(x...);
    this->parent_->set_device(device);
  }
};

template<typename... Ts> class SetVolumeAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(uint8_t, volume)

  void play(Ts... x) override {
    auto volume = this->volume_.value(x...);
    this->parent_->set_volume(volume);
  }
};

template<typename... Ts> class SetEqAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(EqPreset, eq)

  void play(Ts... x) override {
    auto eq = this->eq_.value(x...);
    this->parent_->set_eq(eq);
  }
};

template<typename... Ts> class SetModeAction : public Action<Ts...>, public Parented<DYPlayer> {
 public:
  TEMPLATABLE_VALUE(PlayMode, mode)

  void play(Ts... x) override {
    auto mode = this->mode_.value(x...);
    this->parent_->set_mode(mode);
  }
};

DYPLAYER_SIMPLE_ACTION(SleepAction, sleep)
DYPLAYER_SIMPLE_ACTION(ResetAction, reset)
DYPLAYER_SIMPLE_ACTION(StartAction, start)
DYPLAYER_SIMPLE_ACTION(PauseAction, pause)
DYPLAYER_SIMPLE_ACTION(StopAction, stop)
DYPLAYER_SIMPLE_ACTION(StopInterludeAction, stop_interlude)
DYPLAYER_SIMPLE_ACTION(VolumeUpAction, volume_up)
DYPLAYER_SIMPLE_ACTION(VolumeDownAction, volume_down)

template<typename... Ts> class DYPlayerIsPlayingCondition : public Condition<Ts...>, public Parented<DYPlayer> {
 public:
  bool check(Ts... x) override { return this->parent_->is_playing(); }
};

class DYPlayerFinishedPlaybackTrigger : public Trigger<> {
 public:
  explicit DYPlayerFinishedPlaybackTrigger(DYPlayer *parent) {
    parent->add_on_finished_playback_callback([this]() { this->trigger(); });
  }
};

}  // namespace dyplayer
}  // namespace esphome
