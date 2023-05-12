#ifndef MY_THREAD_H_
#define MY_THREAD_H_

#include <QThread>
#include <functional>
#include <string>

#include "ocr_infer/api/ocr_api.h"

typedef std::function<void(const std::string &, void *)> CallbackFunc;

class MyThread : public QThread {
  Q_OBJECT

 public:
  MyThread();

  int Init(const std::string &config_file, CallbackFunc callback_func,
           void *other);

  void SetImageDir(const QString &image_dir);

 signals:
  void PrintInfo(const QString &info);

 protected:
  void run();

 private:
  OcrInfer ocr_infer_;

  std::string image_dir_;
};

#endif  // MY_THREAD_H_
