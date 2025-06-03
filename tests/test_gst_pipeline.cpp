#include <gtest/gtest.h>
#include <gst/gst.h>

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