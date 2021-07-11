#include "snowloginwindow.h"
#include "ui_snowloginwindow.h"
#include "captchaimagedialog.h"
#include <QNetworkCookieJar>
#include <QWebEngineView>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>

SnowLoginWindow::SnowLoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnowLoginWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("永州电信一卡通"));
    this->networkManager = new QNetworkAccessManager(this);
    QObject::connect(this->ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(onLoginButtonClicked()));
}

SnowLoginWindow::~SnowLoginWindow()
{
    delete this->networkManager;
    delete ui;
}

void SnowLoginWindow::onLoginButtonClicked()
{
    this->ui->loginButton->setDisabled(true);
    this->startRequestingCaptchaImage();
}

void SnowLoginWindow::onRequestCaptchaImageFinished()
{
    qDebug() << "onRequestCaptchaImageFinished()-recv: " << this->captchaImageReply->bytesAvailable();
    qDebug() << "onRequestCaptchaImageFinished()-status: " + this->captchaImageReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    qDebug() << "onRequestCaptchaImageFinished()-setcookie: " + this->captchaImageReply->rawHeader("Set-Cookie");

    this->ui->statusbar->showMessage("Received " + QString::number(this->captchaImageReply->bytesAvailable()) + "bytes from server");

    QNetworkCookieJar *cookieJar = this->networkManager->cookieJar();
    this->cookiesForAIOCP = cookieJar->cookiesForUrl(QUrl("http://134.172.70.230:9080/AIOCP/"));

    qDebug() << "onRequestCaptchaImageFinished()-cookies: ";
    for(int i = 0; i < this->cookiesForAIOCP.length(); i++)
    {
       qDebug() << this->cookiesForAIOCP.at(i).name() << ": "
                << this->cookiesForAIOCP.at(i).value();
    }

    if(this->captchaImageReply->error() == QNetworkReply::NoError)
    {
        QPixmap pixmap;
        pixmap.loadFromData(this->captchaImageReply->readAll());
        CaptchaImageDialog captchaDialog(this->captchaCode,this);
        captchaDialog.setPixmap(pixmap);
        captchaDialog.setModal(true);
        captchaDialog.show();
        captchaDialog.exec();

        this->captchaImageReply->deleteLater();
        this->captchaImageReply = nullptr;
        this->startSendingLoginRequest();
    }
    else
    {
        qDebug() << "onRequestCaptchaImageFinished()-errorString: " + this->captchaImageReply->errorString();
        this->ui->statusbar->showMessage("Error occurred: " + this->captchaImageReply->errorString());
        this->captchaImageReply->deleteLater();
        this->captchaImageReply = nullptr;
    }
}

void SnowLoginWindow::startRequestingCaptchaImage()
{
    QNetworkRequest captchaRequest;
    captchaRequest.setUrl(QUrl(QStringLiteral("http://134.172.70.230:9080/AIOCP/asm/imagecode.action?d=") + QDateTime::currentMSecsSinceEpoch()));
    captchaRequest.setHeader(QNetworkRequest::UserAgentHeader,"Snow-YzTelecomAIOCP-Client me@xuefeng.space");

    this->ui->statusbar->showMessage("Connecting to Server 134.172.70.230:9080...");
    this->captchaImageReply = this->networkManager->get(captchaRequest);
    QObject::connect(this->captchaImageReply, SIGNAL(finished()), this, SLOT(onRequestCaptchaImageFinished()));
}

void SnowLoginWindow::startSendingLoginRequest()
{

}

void SnowLoginWindow::displayAiocpWebView()
{
    QWebEngineView yzTelecomAiocpView;

    QNetworkCookie cookieInfo("JSESSIONID","C673A78928D5CEC780EED9BD933D0A3E.tomcat2");
    QWebEngineCookieStore *cookieStore = yzTelecomAiocpView.page()->profile()->cookieStore();
    cookieStore->setCookie(cookieInfo, QUrl("http://134.172.70.230:9080/"));
    yzTelecomAiocpView.page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    // yzTelecomAiocpView.setUrl(QUrl("http://download.xuefeng.space/checkCookie.html"));
    yzTelecomAiocpView.setUrl(QUrl("http://134.172.70.230:9080/AIOCP/"));
    yzTelecomAiocpView.show();
}

