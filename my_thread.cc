#include "my_thread.h"

#include <QDebug>

MyThread::MyThread() : ocr_infer_() {}

int MyThread::Init(const std::string &config_file, CallbackFunc callback_func,
                   void *other) {
  return ocr_infer_.Init(config_file, callback_func, other);
}

void MyThread::SetImageDir(const QString &image_dir) {
  image_dir_ = image_dir.toStdString();
}

void MyThread::run() {
  if (image_dir_.empty()) {
    qDebug() << "Please set image dir before calling start().";
    return;
  }

  emit PrintInfo("****** 开始运行");

  ocr_infer_.Run(image_dir_);

  emit PrintInfo("****** 运行结束!");
}
