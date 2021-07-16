#include "captchaimagedialog.h"
#include "ui_captchaimagedialog.h"

CaptchaImageDialog::CaptchaImageDialog(QString &_captchaCode, QWidget *parent) :
    captchaCode(_captchaCode),
    QDialog(parent),
    ui(new Ui::CaptchaImageDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    QObject::connect(this->ui->pushButton_Ok, SIGNAL(clicked(bool)), this, SLOT(onPushButtonOkClicked()));
}

void CaptchaImageDialog::setPixmap(QPixmap pixmap)
{
    this->ui->label_CaptchaImage->setPixmap(pixmap);
}

CaptchaImageDialog::~CaptchaImageDialog()
{
    delete ui;
}

void CaptchaImageDialog::onPushButtonOkClicked()
{
    this->captchaCode = this->ui->lineEdit_CaptchaCode->text();
    this->close();
}
