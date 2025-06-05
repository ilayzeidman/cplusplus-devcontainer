#include "gst_pipeline.hpp"
#include <iostream>

GstPipelineWrapper::GstPipelineWrapper(const char* pipeline_str) {
    std::cout << "[GStreamer] Initializing GStreamer..." << std::endl;
    gst_init(nullptr, nullptr);
    GError *error = nullptr;
    std::cout << "[GStreamer] Creating pipeline: " << pipeline_str << std::endl;
    pipeline_ = gst_parse_launch(pipeline_str, &error);
    if (!pipeline_ || error) {
        std::cerr << "[GStreamer] Pipeline creation failed: " << (error ? error->message : "unknown error") << std::endl;
        if (error) g_error_free(error);
    } else {
        std::cout << "[GStreamer] Pipeline created successfully." << std::endl;
    }
}

GstPipelineWrapper::~GstPipelineWrapper() {
    std::cout << "[GStreamer] Destroying pipeline..." << std::endl;
    stop();
    if (pipeline_) {
        gst_object_unref(pipeline_);
        pipeline_ = nullptr;
        std::cout << "[GStreamer] Pipeline destroyed." << std::endl;
    }
}

void GstPipelineWrapper::start() {
    if (pipeline_) {
        std::cout << "[GStreamer] Starting pipeline..." << std::endl;
        gst_element_set_state(pipeline_, GST_STATE_PLAYING);
    } else {
        std::cerr << "[GStreamer] Cannot start: pipeline is null." << std::endl;
    }
}

void GstPipelineWrapper::stop() {
    if (pipeline_) {
        std::cout << "[GStreamer] Stopping pipeline..." << std::endl;
        gst_element_set_state(pipeline_, GST_STATE_NULL);
    } else {
        std::cerr << "[GStreamer] Cannot stop: pipeline is null." << std::endl;
    }
}