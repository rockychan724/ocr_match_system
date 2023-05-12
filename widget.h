#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
// #include "ocr_api.h"
#include "ocr_infer/api/ocr_api.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    static void Print(const std::string &res, void *arg);

signals:
    void SignalsAppendText(const QString &text);

public slots:
    void RunSerial();
    void Run();
    void Init();
    void SlotsAppendText(const QString &text);
    void UploadFile();
    void UploadDir();

private:
    Ui::Widget *ui;
    OcrInfer ocr_engine;

    std::string image_dir = "/home/chenlei/Documents/cnc/testdata/image/";

    static Widget *this_ptr;
};
#endif // WIDGET_H
