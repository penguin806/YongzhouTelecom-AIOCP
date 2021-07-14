#include "snowloginwindow.h"
#include "ui_snowloginwindow.h"
#include "captchaimagedialog.h"
#include <QNetworkCookieJar>
#include <QWebEngineView>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

SnowLoginWindow::SnowLoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnowLoginWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("永州电信一卡通"));
    this->ui->statusbar->showMessage(QStringLiteral("Version: ") + QApplication::applicationVersion() + QStringLiteral(" | 永州分公司-业务运营及IT支撑中心"));
    this->ui->statusbar->setToolTip(QStringLiteral("Email： me@xuefeng.space"));
    this->networkManager = new QNetworkAccessManager(this);
    QObject::connect(this->ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(onLoginButtonClicked()));
}

SnowLoginWindow::~SnowLoginWindow()
{
    delete this->networkManager;
    this->networkManager = nullptr;
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

    this->ui->statusbar->showMessage("Received " + QString::number(this->captchaImageReply->bytesAvailable()) + " bytes from server");

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

void SnowLoginWindow::onLoginFinished()
{
    qDebug() << "onLoginFinished()-recv: " << this->loginReply->bytesAvailable();
    qDebug() << "onLoginFinished()-returnCode: " + this->loginReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "onLoginFinished()-status: " + this->loginReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    this->ui->statusbar->showMessage("Received " + QString::number(this->loginReply->bytesAvailable()) + " bytes from server");

//    QString responseData = QString::fromUtf8(this->loginReply->readAll());
    QJsonDocument loginResultJsonDocument = QJsonDocument::fromJson(this->loginReply->readAll());
    qDebug() << "onLoginFinished()-result: " + loginResultJsonDocument.toJson();
    QJsonObject loginResultJsonObject = loginResultJsonDocument.object();

//    qDebug() << loginResultJsonObject.value("flag");
//    qDebug() << loginResultJsonObject.value("url");
    if(loginResultJsonObject.value("flag").toBool())
    {
        this->loginReply->deleteLater();
        this->loginReply = nullptr;
        this->ui->statusbar->showMessage("Login Success!");
    }
    else
    {
        this->loginReply->deleteLater();
        this->loginReply = nullptr;
        this->ui->statusbar->showMessage("Login Failed! Reason: 1) Username/Password/CaptchaCode incorrect 2) Your account has been banned");
    }
}

void SnowLoginWindow::startRequestingCaptchaImage()
{
    QNetworkRequest captchaRequest;
    captchaRequest.setUrl(QUrl(QStringLiteral("http://134.172.70.230:9080/AIOCP/asm/imagecode.action?d=") + QDateTime::currentMSecsSinceEpoch()));
    captchaRequest.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Snow-YzTelecomAIOCP-Client me@xuefeng.space"));

    this->ui->statusbar->showMessage(QStringLiteral("Connecting to Server 134.172.70.230:9080..."));
    this->captchaImageReply = this->networkManager->get(captchaRequest);
    QObject::connect(this->captchaImageReply, SIGNAL(finished()), this, SLOT(onRequestCaptchaImageFinished()));
}

void SnowLoginWindow::startSendingLoginRequest()
{
    QNetworkRequest loginRequest;
    loginRequest.setUrl(QUrl("http://134.172.70.230:9080/AIOCP/asm/login.action"));
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    loginRequest.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Snow-YzTelecomAIOCP-Client me@xuefeng.space"));
    QVariant var;
    var.setValue(this->cookiesForAIOCP);
    loginRequest.setHeader(QNetworkRequest::CookieHeader,var);

    QUrlQuery queryParams;
    queryParams.addQueryItem("imagecode", this->captchaCode);
    queryParams.addQueryItem("username", this->ui->userEdit->text());
    queryParams.addQueryItem("password", this->ui->passwordEdit->text());
    queryParams.addQueryItem("logintype", QString::number(22));

    this->ui->statusbar->showMessage("Sending login request...");
    this->loginReply = this->networkManager->post(loginRequest, queryParams.query().toUtf8());

    this->captchaCode.clear();
    QObject::connect(this->loginReply, SIGNAL(finished()), this, SLOT(onLoginFinished()));
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

