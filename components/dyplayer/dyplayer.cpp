#include "dyplayer.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dyplayer {

static const char *const TAG = "dyplayer";

void DYPlayer::next() {
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Playing next track");
  this->send_cmd_(0x06);
}

void DYPlayer::previous() {
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Playing previous track");
  this->send_cmd_(0x05);
}

void DYPlayer::interlude_file(uint16_t file) {
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Interlude file %d", file);
  this->send_cmd_(0x16, file);
}

void DYPlayer::play_file(uint16_t file) {
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Playing file %d", file);
  this->send_cmd_(0x07, file);
}

void DYPlayer::select_file(uint16_t file) {
  ESP_LOGD(TAG, "Selecting file %d", file);
  this->send_cmd_(0x1f, file);
}

void DYPlayer::volume_up() {
  ESP_LOGD(TAG, "Increasing volume");
  this->send_cmd_(0x14);
}

void DYPlayer::volume_down() {
  ESP_LOGD(TAG, "Decreasing volume");
  this->send_cmd_(0x15);
}

void DYPlayer::set_device(Device device) {
  ESP_LOGD(TAG, "Setting device to %d", device);
  this->send_cmd_(0x0b, (uint8_t) device);
}

void DYPlayer::set_volume(uint8_t volume) {
  ESP_LOGD(TAG, "Setting volume to %d", volume);
  this->send_cmd_(0x13, volume);
}

void DYPlayer::set_eq(EqPreset preset) {
  ESP_LOGD(TAG, "Setting EQ to %d", preset);
  this->send_cmd_(0x1a, (uint8_t) preset);
}

void DYPlayer::sleep() {
  ESP_LOGW(TAG, "NOT IMPLEMENTED: DF-Player leftover"); return; // TODO
  this->ack_reset_is_playing_ = true;
  ESP_LOGD(TAG, "Putting DYPlayer to sleep");
  this->send_cmd_(0x0A);
}

void DYPlayer::reset() {
  ESP_LOGW(TAG, "NOT IMPLEMENTED: DF-Player leftover"); return; // TODO
  this->ack_reset_is_playing_ = true;
  ESP_LOGD(TAG, "Resetting DYPlayer");
  this->send_cmd_(0x0C);
}

void DYPlayer::start() {
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Starting playback");
  this->send_cmd_(0x02);
}

void DYPlayer::pause() {
  this->ack_reset_is_playing_ = true;
  ESP_LOGD(TAG, "Pausing playback");
  this->send_cmd_(0x03);
}

void DYPlayer::stop() {
  this->ack_reset_is_playing_ = true;
  ESP_LOGD(TAG, "Stopping playback");
  this->send_cmd_(0x04);
}

void DYPlayer::random() {
  ESP_LOGW(TAG, "NOT IMPLEMENTED: DF-Player leftover"); return; // TODO
  this->ack_set_is_playing_ = true;
  ESP_LOGD(TAG, "Playing random file");
  this->send_cmd_(0x18);
}

void DYPlayer::play_folder(uint16_t folder, uint16_t file) {
  ESP_LOGW(TAG, "NOT IMPLEMENTED: DF-Player leftover"); return; // TODO
  ESP_LOGD(TAG, "Playing file %d in folder %d", file, folder);
  if (folder < 100 && file < 256) {
    this->ack_set_is_playing_ = true;
    this->send_cmd_(0x0F, (uint8_t) folder, (uint8_t) file);
  } else if (folder <= 15 && file <= 3000) {
    this->ack_set_is_playing_ = true;
    this->send_cmd_(0x14, (uint16_t) ((((uint16_t) folder) << 12) | file));
  } else {
    ESP_LOGE(TAG, "Cannot play folder %d file %d.", folder, file);
  }
}

void DYPlayer::send_cmd_(uint8_t cmd, uint8_t *data, uint8_t len) {
  uint8_t p = 3;
  uint8_t buffer[10]{0xAA, cmd, len};
  uint8_t checksum = 0;
  if (len > 2) {
    ESP_LOGW(TAG, "Unimplemented: Cmd with len>2 need bigger buf and testing");
    return;
  }
  for (uint8_t i = 1; i <= len; i++)
    buffer[p++] = data[i - 1];
  for (uint8_t j = 0; j < p; j++)
    checksum += buffer[j];
  buffer[p++] = checksum;

  this->sent_cmd_ = cmd;

  ESP_LOGV(TAG, "Send Command %#02x total length %d", cmd, p);
  this->write_array(buffer, p);
}

void DYPlayer::loop() {
  // Read message
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);

    if (this->read_pos_ == DYPLAYER_READ_BUFFER_LENGTH)
      this->read_pos_ = 0;

    switch (this->read_pos_) {
      case 0:  // Start mark
        if (byte != 0xAA)
          continue;
        break;
      case 2:  // Buffer length
        if (byte > 2) {
          ESP_LOGW(TAG, "Invalid Buffer length, got %#02x", byte);
          this->read_pos_ = 0;
          continue;
        }
        this->in_len_ = byte;
        break;
    }
    if (this->read_pos_ == 3 + this->in_len_) {
      uint8_t checksum = 0;
      for (uint8_t j = 0; j < this->read_pos_; j++)
        checksum += read_buffer_[j];
      if (byte != checksum) {
        ESP_LOGW(TAG, "Invalid checksum, got %#02x, expected %#02x", byte, checksum);
        this->read_pos_ = 0;
        continue;
      }
#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
      char byte_sequence[100];
      byte_sequence[0] = '\0';
      for (size_t i = 0; i < this->read_pos_ + 1; ++i) {
        snprintf(byte_sequence + strlen(byte_sequence), sizeof(byte_sequence) - strlen(byte_sequence), "%02X ",
                 this->read_buffer_[i]);
      }
      ESP_LOGVV(TAG, "Received byte sequence: %s", byte_sequence);
#endif
      // Parse valid received command
      uint8_t cmd = this->read_buffer_[1];
      uint16_t argument = 0;
      switch (this->in_len_) {
        case 1:
          argument = this->read_buffer_[3];
          break;
        case 2:
          argument = (this->read_buffer_[3] << 8) | this->read_buffer_[4];
          break;
      }

      ESP_LOGV(TAG, "Received message cmd: %#02x arg %#04x", cmd, argument);

      switch (cmd) {
        case 0x01:
          ESP_LOGI(TAG, "Play status is %d", argument);
          break;
        case 0x09:
          ESP_LOGI(TAG, "Current online drive is %d", argument);
          break;
        case 0x0A:
          ESP_LOGI(TAG, "Current play drive is %d", argument);
          break;
        case 0x0C:
          ESP_LOGI(TAG, "Number of songs is %d", argument);
          break;
        case 0x0D:
          ESP_LOGI(TAG, "Current song is %d", argument);
          break;
        case 0x11:
          ESP_LOGI(TAG, "Folder dir song is %d", argument);
          break;
        case 0x12:
          ESP_LOGI(TAG, "Folder number of song is %d", argument);
          break;
/* DF-Player leftover TODO
        case 0x3A:
          if (argument == 1) {
            ESP_LOGI(TAG, "USB loaded");
          } else if (argument == 2) {
            ESP_LOGI(TAG, "TF Card loaded");
          }
          break;
        case 0x3B:
          if (argument == 1) {
            ESP_LOGI(TAG, "USB unloaded");
          } else if (argument == 2) {
            ESP_LOGI(TAG, "TF Card unloaded");
          }
          break;
        case 0x3F:
          if (argument == 1) {
            ESP_LOGI(TAG, "USB available");
          } else if (argument == 2) {
            ESP_LOGI(TAG, "TF Card available");
          } else if (argument == 3) {
            ESP_LOGI(TAG, "USB, TF Card available");
          }
          break;
        case 0x40:
          ESP_LOGV(TAG, "Nack");
          this->ack_set_is_playing_ = false;
          this->ack_reset_is_playing_ = false;
          switch (argument) {
            case 0x01:
              ESP_LOGE(TAG, "Module is busy or uninitialized");
              break;
            case 0x02:
              ESP_LOGE(TAG, "Module is in sleep mode");
              break;
            case 0x03:
              ESP_LOGE(TAG, "Serial receive error");
              break;
            case 0x04:
              ESP_LOGE(TAG, "Checksum incorrect");
              break;
            case 0x05:
              ESP_LOGE(TAG, "Specified track is out of current track scope");
              this->is_playing_ = false;
              break;
            case 0x06:
              ESP_LOGE(TAG, "Specified track is not found");
              this->is_playing_ = false;
              break;
            case 0x07:
              ESP_LOGE(TAG, "Insertion error (an inserting operation only can be done when a track is being played)");
              break;
            case 0x08:
              ESP_LOGE(TAG, "SD card reading failed (SD card pulled out or damaged)");
              break;
            case 0x09:
              ESP_LOGE(TAG, "Entered into sleep mode");
              this->is_playing_ = false;
              break;
          }
          break;
        case 0x41:
          ESP_LOGV(TAG, "Ack ok");
          this->is_playing_ |= this->ack_set_is_playing_;
          this->is_playing_ &= !this->ack_reset_is_playing_;
          this->ack_set_is_playing_ = false;
          this->ack_reset_is_playing_ = false;
          break;
        case 0x3C:
          ESP_LOGV(TAG, "Playback finished (USB drive)");
          this->is_playing_ = false;
          this->on_finished_playback_callback_.call();
        case 0x3D:
          ESP_LOGV(TAG, "Playback finished (SD card)");
          this->is_playing_ = false;
          this->on_finished_playback_callback_.call();
          break;
*/
        default:
          ESP_LOGE(TAG, "Received unknown cmd %#02x arg %#04x", cmd, argument);
      }
      this->sent_cmd_ = 0;
      this->read_pos_ = 0;
      continue;
    }
    this->read_buffer_[this->read_pos_] = byte;
    this->read_pos_++;
  }
}
void DYPlayer::dump_config() {
  ESP_LOGCONFIG(TAG, "DYPlayer:");
  this->check_uart_settings(9600);
}

}  // namespace dyplayer
}  // namespace esphome
