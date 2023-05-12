#include "widget.h"
#include "ui_widget.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>
#include "opencv2/opencv.hpp"
// #include "data_type.h"
#include "ocr_infer/api/data_type.h"

static std::vector<std::string> GetFileName(
    const std::vector<std::string> &files) {
  std::vector<std::string> names;
  for (const auto &file : files) {
    int index1 = file.find_last_of("/");
    int index2 = file.find_last_of(".");
    names.emplace_back(file.substr(index1 + 1, index2 - index1 - 1));
  }
  return names;
}

static size_t ReadImages(const std::string &images_path,
                         std::vector<cv::Mat> &images,
                         std::vector<std::string> &names) {
  std::vector<cv::String> files;
  cv::glob(images_path + "/*.jpg", files, false);
  std::shuffle(files.begin(), files.end(), std::default_random_engine(9));
  size_t count = files.size();
  for (size_t i = 0; i < count; i++) {
    cv::Mat img = cv::imread(files[i], cv::IMREAD_COLOR);
    images.emplace_back(img);
  }
  names = GetFileName(files);
  printf("\nThere are %lu images\n\n", count);
  return count;
}

Widget* Widget::this_ptr = nullptr;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget), ocr_engine()
{

    ui->setupUi(this);

    connect(ui->pushButton_init, SIGNAL(clicked()), this, SLOT(Init()));
    connect(ui->pushButton_run, SIGNAL(clicked()), this, SLOT(Run()));
    connect(ui->pushButton_uploadfile, SIGNAL(clicked()), this, SLOT(UploadFile()));
    connect(ui->pushButton_uploaddir, SIGNAL(clicked()), this, SLOT(UploadDir()));
    connect(this, SIGNAL(SignalsAppendText(QString)), this, SLOT(SlotsAppendText(QString)));

//    QMovie *movie = new QMovie("/home/chenlei/Documents/QtProjects/helloqtcreator/demo.jpg");
//    ui->label_detResult->setMovie(movie);
//    movie->start();

    QString str = "请点击“初始化”按钮来初始化程序";
    qDebug() << str;
    ui->textEdit_recResult->append(str);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::Print(const std::string &res, void *arg)
{
//    std::cout << res;
//    this_ptr->ui->textEdit_recResult->append(res.c_str());
    emit this_ptr->SignalsAppendText(res.c_str());
//    auto ptr = (Widget*)arg;
//    emit ptr->SlotsAppendText(res.c_str());
}

void Widget::Init()
{
    this_ptr = this;
    std::string config_file = "/home/chenlei/Documents/cnc/ocr_infer/data/config_cnc.ini";
    ocr_engine.Init(config_file, Widget::Print, this);
//    worker.Init(config_file, (void *)Widget::Print, this);
    QString str = "初始化完成！";
    qDebug() << str;
    ui->textEdit_recResult->append(str);
}

void Widget::Run()
{
    QString str = "****** Ready to run.";
    qDebug() << str;
    ui->textEdit_recResult->append(str);

    ocr_engine.Run(image_dir);
//    worker.Run(image_dir);

    str = "****** Run over!";
    qDebug() << str;
    ui->textEdit_recResult->append(str);
}

void Widget::RunSerial()
{
    QString str = "****** Ready to run.";
    qDebug() << str;
    ui->textEdit_recResult->append(str);

//    ocr_engine.Run(image_dir);
//    worker.Run(image_dir);

    int detect_batch_size_ = 50;
    std::vector<cv::Mat> images;
    std::vector<std::string> names;
    int count = ReadImages(image_dir, images, names);

    int det_batch_num = std::ceil(double(count) / detect_batch_size_);
    int begin_index = 0;
    for (int i = 0; i < det_batch_num; i++) {
        std::shared_ptr<Input> det_in = std::make_shared<Input>();
        int end_index = begin_index + detect_batch_size_ >= count
                            ? count
                            : begin_index + detect_batch_size_;
        det_in->names.assign(names.begin() + begin_index,
                             names.begin() + end_index);
        det_in->images.assign(images.begin() + begin_index,
                              images.begin() + end_index);
        begin_index = end_index;

        std::string res = ocr_engine.Run(det_in);
        qDebug(res.c_str());
        ui->textEdit_recResult->append(res.c_str());
    }

    str = "****** Run over!";
    qDebug() << str;
    ui->textEdit_recResult->append(str);
}

void Widget::SlotsAppendText(const QString &text)
{
//    qDebug() << "debug in ****** hhh ******";
//    ui->textEdit_recResult->append("****** hhh ******");
    qDebug() << text;
    ui->textEdit_recResult->append(text);
    //    ui->label_detResult->setText(text);
}

void Widget::UploadFile()
{
    QString filename = QFileDialog::getOpenFileName(
                           this,
                           "选择文件",
                           "D:/",
                           "文档(*.jpg *.jpeg *.png);;All files(*.*)");
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
    QImage img_qt(static_cast<const unsigned char *>(img_rgb.data), img_rgb.cols, img_rgb.rows, img_rgb.step, QImage::Format_RGB888);

    ui->label_detResult->setPixmap(QPixmap::fromImage(img_qt));
    ui->label_detResult->setScaledContents(true);
}

void Widget::UploadDir()
{
    ui->textEdit_recResult->append("Upload dir");

    QString dirname = QFileDialog::getExistingDirectory(
                this,
                "select directory",
                "D:/",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirname.isEmpty()) {
        qDebug() << "Failed to open the directory!";
    } else {
        image_dir = dirname.toStdString();
        qDebug() << image_dir.c_str();
    }
}
