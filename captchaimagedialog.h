#ifndef CAPTCHAIMAGEDIALOG_H
#define CAPTCHAIMAGEDIALOG_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class CaptchaImageDialog;
}

class CaptchaImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaptchaImageDialog(QString &_captchaCode, QWidget *parent = nullptr);
    ~CaptchaImageDialog();
    void setPixmap(QPixmap pixmap);

private slots:
    void onPushButtonOkClicked();

private:
    Ui::CaptchaImageDialog *ui;
    QString &captchaCode;
};

#endif // CAPTCHAIMAGEDIALOG_H
