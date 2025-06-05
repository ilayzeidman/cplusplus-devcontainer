#ifndef GST_PIPELINE_HPP
#define GST_PIPELINE_HPP

#include <gst/gst.h>
#include <string>

class GstPipelineWrapper {
    public:
        GstPipelineWrapper(const char* pipeline_str);
        ~GstPipelineWrapper();

        void start();
        void stop();
        //void set_bitrate(const std::string& encoder_name, guint bitrate);
    private:
        GstElement* pipeline_;
};

#endif // GST_PIPELINE_HPP