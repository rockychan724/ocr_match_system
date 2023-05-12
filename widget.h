#ifndef WIDGET_H
#define WIDGET_H

#include <memory>

#include <QWidget>
// #include "ocr_api.h"
#include "my_thread.h"
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

signals:
    void SignalAppendText(const QString &text);

public slots:
    void SlotRun();
    void SlotInit();
    void SlotUploadFile();
    void SlotUploadDir();
    void SlotAppendText(const QString &text);

    void SlotPrintInfo(const QString &info);

private:
    Ui::Widget *ui;

    static Widget *this_ptr;

    std::unique_ptr<MyThread> my_thread;
    QString image_dir = "/home/chenlei/Documents/cnc/testdata/image/";

    static void Print(const std::string &res, void *other);
};
#endif // WIDGET_H
