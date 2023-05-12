#include "widget.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>

#include "opencv2/opencv.hpp"
#include "ui_widget.h"
// #include "data_type.h"
#include "ocr_infer/api/data_type.h"

Widget *Widget::this_ptr = nullptr;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);

  connect(ui->pushButton_init, SIGNAL(clicked()), this, SLOT(SlotInit()));
  connect(ui->pushButton_run, SIGNAL(clicked()), this, SLOT(SlotRun()));
  connect(ui->pushButton_uploadfile, SIGNAL(clicked()), this,
          SLOT(SlotUploadFile()));
  connect(ui->pushButton_uploaddir, SIGNAL(clicked()), this,
          SLOT(SlotUploadDir()));
  connect(this, SIGNAL(SignalAppendText(QString)), this,
          SLOT(SlotAppendText(QString)));

  //    QMovie *movie = new
  //    QMovie("/home/chenlei/Documents/QtProjects/helloqtcreator/demo.jpg");
  //    ui->label_detResult->setMovie(movie);
  //    movie->start();

  QString info = "****** 请点击“初始化”按钮来加载模型";
  qDebug() << info;
  ui->textEdit_recResult->append(info);

  my_thread = std::make_unique<MyThread>();
  my_thread->SetImageDir(image_dir);
  connect(my_thread.get(), SIGNAL(PrintInfo(QString)), this,
          SLOT(SlotPrintInfo(QString)));
}

Widget::~Widget() { delete ui; }

void Widget::SlotInit() {
  QString info = "****** 初始化失败，请检查模型路径！";
  this_ptr = this;
  std::string config_file =
      "/home/chenlei/Documents/cnc/ocr_infer/data/config_cnc.ini";
  if (my_thread->Init(config_file, Widget::Print, this) != 0) {
    qDebug() << info;
    ui->textEdit_recResult->append(info);
    return;
  }

  info = "****** 初始化成功！";
  qDebug() << info;
  ui->textEdit_recResult->append(info);
}

/**
 * Attention:
 * 注意，如果函数运行时间很长，一定不能直接调用该函数，不然会造成界面的主线程阻塞，
 * 导致界面卡住，无法显示效果不理想。正确做法是使用多线程将该函数放到子线程中去执行。
 * 
 * 趟过的坑：
 * OcrInfer::Run 在执行过程中会调用回调函数在界面中实时更新程序的处理结果，
 * 最开始的做法是直接在 SlotRun 函数里执行 OcrInfer::Run，但由于 OcrInfer::Run
 * 执行时间较长，这会导致主界面线程卡住，无法实时显示中间结果。正确的做法开启新的线程
 * 来运行 OcrInfer::Run。
 * 
 */
void Widget::SlotRun() {
  my_thread->SetImageDir(image_dir);
  my_thread->start();
}

void Widget::SlotUploadFile() {
  QString filename = QFileDialog::getOpenFileName(
      this, "选择文件", "D:/", "文档(*.jpg *.jpeg *.png);;All files(*.*)");
  if (filename.isEmpty()) {
    qDebug() << "Failed to open the file!";
    return;
  }

  qDebug() << filename;

  cv::Mat img_cv = cv::imread(filename.toStdString());

  if (img_cv.empty()) {
    QMessageBox::information(this, "info", "Can't read file!");
    return;
  }

  cv::Mat img_rgb;
  cv::cvtColor(img_cv, img_rgb, cv::COLOR_BGR2RGB);
  QImage img_qt(static_cast<const unsigned char *>(img_rgb.data), img_rgb.cols,
                img_rgb.rows, img_rgb.step, QImage::Format_RGB888);

  ui->label_detResult->setPixmap(QPixmap::fromImage(img_qt));
  ui->label_detResult->setScaledContents(true);
}

void Widget::SlotUploadDir() {
  ui->textEdit_recResult->append("Upload dir");

  QString dirname = QFileDialog::getExistingDirectory(
      this, "select directory", "D:/",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (dirname.isEmpty()) {
    qDebug() << "Failed to open the directory!";
  } else {
    image_dir = dirname;
    qDebug() << image_dir;
  }
}

void Widget::SlotAppendText(const QString &text) {
  qDebug() << text;
  ui->textEdit_recResult->append(text);
}

void Widget::SlotPrintInfo(const QString &info) {
  qDebug() << info;
  ui->textEdit_recResult->append(info);
}

// Callback function
void Widget::Print(const std::string &res, void *other) {
  if (this_ptr == nullptr) {
    qDebug() << "****** 请在运行前先执行初始化！";
    return;
  }
  emit this_ptr->SignalAppendText(res.c_str());
}
