#include <gtest/gtest.h>
#include <gst/gst.h>
#include <thread>
#include "utils/pipeline_descriptions.hpp"


TEST(MypassthroughTest, PipelineInit) {
    gst_init(nullptr, nullptr);

    // Crée un pipeline simple avec mypassthrough
    GstElement *pipeline = gst_parse_launch(
        "fakesrc num-buffers=1 ! mypassthrough ! fakesink", nullptr);

    ASSERT_NE(pipeline, nullptr);

    // Joue le pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Attends la fin du pipeline
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE,
        (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

    ASSERT_NE(msg, nullptr);

    // Nettoyage
    if (msg) gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void gstreamer_set_bitrate(GstElement *pipeline, const guint trackIndex, guint bitrate) {
    GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "encode");
    guint old_bitrate = 0;
    if (enc != NULL) {
        g_object_get(enc, "bitrate", &old_bitrate, NULL);
        g_object_set(enc, "bitrate", bitrate, NULL);
        GST_INFO("set bitrate of track %d from %d to %d successfully!", trackIndex, old_bitrate, bitrate);
        gst_object_unref(enc);
    }
}

TEST(MypassthroughTest, ChangeBitrate) {
    gst_init(nullptr, nullptr);

    // Pipeline avec x264enc nommé "encode"
    GstElement *pipeline = gst_parse_launch(
        "videotestsrc num-buffers=10 ! videoconvert ! x264enc name=encode tune=zerolatency bitrate=2000 speed-preset=superfast ! fakesink",
        nullptr);

    ASSERT_NE(pipeline, nullptr);

    // Met le pipeline en état PLAYING pour s'assurer que tous les éléments sont prêts
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Attend que le pipeline atteigne l'état PLAYING
    GstStateChangeReturn ret = gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    ASSERT_TRUE(ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC);

    // Change le bitrate à 1000
    gstreamer_set_bitrate(pipeline, 0, 1000);

    GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "encode");
    ASSERT_NE(enc, nullptr);
    guint current_bitrate = 0;
    g_object_get(enc, "bitrate", &current_bitrate, NULL);
    EXPECT_EQ(current_bitrate, 1000u);
    gst_object_unref(enc);

    // Nettoyage
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

TEST(MypassthroughTest, ChangeBitrateVisual) {
    gst_init(nullptr, nullptr);

    // Pipeline qui affiche la vidéo encodée/décodée dans une fenêtre
    GstElement *pipeline = gst_parse_launch(
    "videotestsrc is-live=false num-buffers=1000 ! video/x-raw,framerate=30/1 ! videoconvert ! x264enc tune=zerolatency bitrate=2000 key-int-max=30 ! mp4mux ! filesink location=output1.mp4",
        NULL);

    ASSERT_NE(pipeline, nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Attend que le pipeline atteigne l'état PLAYING
    GstStateChangeReturn ret = gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    ASSERT_TRUE(ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC);

    // Attend 2 secondes à bitrate 2000
    g_usleep(2 * G_USEC_PER_SEC);

    // Change le bitrate à 500 (devrait dégrader la qualité)
    gstreamer_set_bitrate(pipeline, 0, 500);

    // Attend 3 secondes pour voir l'effet
    g_usleep(10 * G_USEC_PER_SEC);

    // Nettoyage
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void run_pipeline_with_bitrate(const char* output_file, int bitrate_kbps) {
    gchar *pipeline_desc = g_strdup_printf(
        "videotestsrc pattern=ball is-live=false num-buffers=300 ! video/x-raw,framerate=30/1 ! videoconvert ! x264enc tune=zerolatency bitrate=%d key-int-max=30 ! mp4mux ! filesink location=%s",
        bitrate_kbps,
        output_file);

    GstElement *pipeline = gst_parse_launch(pipeline_desc, NULL);
    ASSERT_NE(pipeline, nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus(pipeline);
    bool done = false;
    while (!done) {
        GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        if (msg != nullptr) {
            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR:
                    GError *err;
                    gchar *debug_info;
                    gst_message_parse_error(msg, &err, &debug_info);
                    std::cerr << "Error received: " << err->message << std::endl;
                    g_clear_error(&err);
                    g_free(debug_info);
                    done = true;
                    break;

                case GST_MESSAGE_EOS:
                    std::cout << "End-Of-Stream reached." << std::endl;
                    done = true;
                    break;

                default:
                    break;
            }
            gst_message_unref(msg);
        }
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_free(pipeline_desc);
}

TEST(MypassthroughTest, ChangeBitrateVisual_LiveWindow) {
    gst_init(nullptr, nullptr);

    // Pipeline with live display
    GstElement *pipeline = gst_parse_launch(
        "videotestsrc pattern=ball is-live=true ! video/x-raw,framerate=30/1 "
        "! videoconvert "
        "! x264enc name=encode tune=zerolatency bitrate=2000 key-int-max=30 "
        "! avdec_h264 "
        "! videoconvert "
        "! ximagesink",
        NULL);

    ASSERT_NE(pipeline, nullptr);

    // Get encoder element
    GstElement *encoder = gst_bin_get_by_name(GST_BIN(pipeline), "encode");
    ASSERT_NE(encoder, nullptr);

    // Set pipeline to PLAYING
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait until pipeline is playing
    GstStateChangeReturn ret = gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    ASSERT_TRUE(ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC);

    std::cout << "Pipeline running at bitrate 2000..." << std::endl;

    // Let it play at 2000 kbps for 5 seconds
    g_usleep(5 * G_USEC_PER_SEC);

    std::cout << "Changing bitrate to 500..." << std::endl;

    // Change bitrate property
    g_object_set(G_OBJECT(encoder), "bitrate", 500, NULL);

    // Let it play at 500 kbps for 10 seconds
    g_usleep(10 * G_USEC_PER_SEC);

    std::cout << "Test done, stopping pipeline..." << std::endl;

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(encoder);
    gst_object_unref(pipeline);
}

TEST(MypassthroughTest, ChangeBitrateVisual_LiveWindow_Threaded) {
    gst_init(nullptr, nullptr);

    // Pipeline with live display
    // Define the pipeline description in a header or constants file and include it here
    extern const char* LIVE_WINDOW_PIPELINE_DESC;
    GstElement *pipeline = gst_parse_launch(LIVE_WINDOW_PIPELINE_DESC, NULL);

    ASSERT_NE(pipeline, nullptr);

    // Get encoder element
    GstElement *encoder = gst_bin_get_by_name(GST_BIN(pipeline), "encode");
    ASSERT_NE(encoder, nullptr);

    // Set pipeline to PLAYING
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait until pipeline is playing
    GstStateChangeReturn ret = gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    ASSERT_TRUE(ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC);

    std::cout << "Pipeline running at bitrate 6000..." << std::endl;

    // Create and run GMainLoop in background thread
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    ASSERT_NE(loop, nullptr);

    std::thread gst_thread([loop]() {
        g_main_loop_run(loop);
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Changing bitrate to 200..." << std::endl;

    // Change bitrate property
    g_object_set(G_OBJECT(encoder), "bitrate", 200, NULL);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Changing bitrate to 6000..." << std::endl;

    g_object_set(G_OBJECT(encoder), "bitrate", 6000, NULL);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Test done, stopping pipeline..." << std::endl;

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    g_main_loop_quit(loop);
    gst_thread.join();
    g_main_loop_unref(loop);

    gst_object_unref(encoder);
    gst_object_unref(pipeline);
}

