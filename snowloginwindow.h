#ifndef SNOWLOGINWINDOW_H
#define SNOWLOGINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QWebEngineView>

QT_BEGIN_NAMESPACE
namespace Ui { class SnowLoginWindow; }
QT_END_NAMESPACE

class SnowLoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    SnowLoginWindow(QWidget *parent = nullptr);
    ~SnowLoginWindow();

private slots:
    void onLoginButtonClicked();
    void onAboutButtonClicked();
    void onRequestCaptchaImageFinished();
    void onLoginFinished();
    void displayAiocpWebView();

private:
    void startRequestingCaptchaImage();
    void startSendingLoginRequest();

    Ui::SnowLoginWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *captchaImageReply;
    QNetworkReply *loginReply;
    QList<QNetworkCookie> cookiesForAIOCP;
    QString captchaCode;
    QWebEngineView yzTelecomAiocpView;
};
#endif // SNOWLOGINWINDOW_H
