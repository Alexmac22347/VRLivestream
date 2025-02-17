/**
 * @file encoder_context.cpp
 * @brief Implementation of the EncoderContext class
 */

#include "encoding/encoder_context.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
}

#include <stdexcept>

#include "common/codec_exception.h"

namespace codec {
namespace encoder {

EncoderContext::EncoderContext(const int fps, const int width, const int height, char *const codec_speed, const int codec_type)
{
  AVCodecID codec_id;
  switch (codec_id) {
      case H264: codec_id = AVCodecID::AV_CODEC_ID_H264;
      case H265: codec_id = AVCodecID::AV_CODEC_ID_H265;
      default: codec_id = AVCodecID::AV_CODEC_ID_H264;
  }
  codec_ptr_ = avcodec_find_encoder(codec_id);
  if (!codec_ptr_)
    throw std::runtime_error("Encoder codec not found");

  codec_context_ptr_ = avcodec_alloc_context3(codec_ptr_);
  if (!codec_context_ptr_)
    throw std::runtime_error("Could not allocate codec context");

  codec_context_ptr_->width = width;
  codec_context_ptr_->height = height;
  codec_context_ptr_->time_base = AVRational {1, fps};
  codec_context_ptr_->framerate = AVRational {fps, 1};
  codec_context_ptr_->pix_fmt = PIX_FMT;
  codec_context_ptr_->gop_size = 25;  // number of frames for 1 i-frame
  codec_context_ptr_->max_b_frames = 0;  // No B-Frames, which need future frames
  codec_context_ptr_->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
  av_opt_set(codec_context_ptr_->priv_data, "preset", codec_speed, 0);
  av_opt_set(codec_context_ptr_->priv_data, "tune", "zerolatency", 0);

  int success = avcodec_open2(codec_context_ptr_, codec_ptr_, nullptr);
  if (success < 0)
    throw CodecException(success, "Could not open encoder context");
}

EncoderContext::~EncoderContext()
{
  avcodec_close(codec_context_ptr_);
  av_free(codec_context_ptr_);
  av_free(codec_ptr_);
}

}  // encoder
}  // namespace codec
