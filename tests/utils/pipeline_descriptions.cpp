#include "pipeline_descriptions.hpp"

const char* LIVE_WINDOW_PIPELINE_DESC =
    "videotestsrc pattern=smpte is-live=true ! timeoverlay ! video/x-raw,framerate=30/1 "
    "! videoconvert "
    "! x264enc name=encode tune=zerolatency bitrate=6000 key-int-max=30 "
    "! avdec_h264 "
    "! videoconvert "
    "! ximagesink";