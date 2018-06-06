#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <boost/filesystem.hpp>
#include <opencv/cv.hpp>
#include <queue>
#include <string>

#include <thread>

#include "helper.h"
#include "setup.h"
namespace {
namespace fs = boost::filesystem;
}

class ImageWriter {
public:
    ImageWriter() {}

    void push(const cv::Mat& img) { m_queue.push(img); }
    void clear() {
        clearQueue(m_queue);
        m_run = false;
        m_finishedWriting = false;
    }

    void startWriting(const fs::path& path, const std::string& prefix) {
        if (!m_run) {
            m_path = path;
            m_prefix = prefix;
            m_run = true;
            std::thread t(&ImageWriter::writeThreaded, this);
            t.detach();
        }
    }

    void finishWriting() {
        // Basically a join() function for writeThreaded()
        m_run = false;
        while (!m_finishedWriting)
            ;
    }

private:
    void writeThreaded() {
        // Will monitor m_queue until m_run is deasserted
        // When deasserted, m_queue will be emptied
        while (m_run | !m_queue.empty()) {
            if (!m_queue.empty()) {
                std::string filepath =
                    (m_path / fs::path(m_prefix + "_" + std::to_string(index) + ".png")).string();
                cv::imwrite(filepath, m_queue.front());
                index++;
                m_queue.pop();
            }
        }
        m_finishedWriting = true;
    }

    bool m_run = false;
    bool m_finishedWriting = false;

    std::queue<cv::Mat> m_queue;

    Setup m_setup;

    fs::path m_path;
    std::string m_prefix;

    int index = 0;
};

#endif  // IMAGEWRITER_H
