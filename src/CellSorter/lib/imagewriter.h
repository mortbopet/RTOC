#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <boost/filesystem.hpp>
#include <opencv/cv.hpp>
#include <queue>
#include <string>

#include <thread>

#include "helper.h"
#include "setup.h"

#define NDEBUG
#include "../external/readerwriterqueue/readerwriterqueue.h"

namespace {
namespace fs = boost::filesystem;
}

class ImageWriter {
public:
    ImageWriter() {}

    void push(const cv::Mat& img) { m_queue.enqueue(img.clone()); }
    void clear() {
        // clear queue
        bool r = true;
        while (r)
            r = m_queue.pop();
        m_running = false;
        m_finishedWriting = false;
        m_targetImageCount = -1;
    }

    void startWriting(const fs::path& path, const std::string& prefix) {
        if (!m_running) {
            m_running = true;
            m_path = path;
            m_prefix = prefix;
            std::thread t(&ImageWriter::writeThreaded, this);
            t.detach();
        }
    }

    void finishWriting(int targetImageCount) {
        // Set target images that we require to be written to disk and wait for finished image
        // writing
        m_targetImageCount = targetImageCount;
        while (!m_finishedWriting)
            ;
    }

private:
    void writeThreaded() {
        // Will monitor m_queue until m_run is deasserted
        // When deasserted, m_queue will be emptied
        cv::Mat front;
        bool queueHasValue = false;
        // Monitor queue until
        //  1. targetImageCount has been set (this will be set when the imageWriter is told to stop
        //  executing
        //  and
        //  2. our index (image count) is equals to the target image count
        while (!(m_targetImageCount >= 0 && m_targetImageCount == m_index)) {
            queueHasValue = m_queue.wait_dequeue_timed(front, std::chrono::seconds(5));
            if (queueHasValue) {
                std::string filepath =
                    (m_path / fs::path(m_prefix + "_" + std::to_string(m_index) + ".png")).string();
                cv::imwrite(filepath, front);
                m_index++;
            } else {
                // error in writing images - did not find the expected amt of images in the queue,
                // as what the analyzer told the ImageWriter to write!
                break;
            }
        }
        m_finishedWriting = true;
    }

    bool m_finishedWriting = false;
    bool m_running = false;

    moodycamel::BlockingReaderWriterQueue<cv::Mat> m_queue;

    Setup m_setup;

    fs::path m_path;
    std::string m_prefix;

    int m_index = 0;
    int m_targetImageCount = -1;
};

#endif  // IMAGEWRITER_H
