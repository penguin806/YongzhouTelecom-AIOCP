#ifndef SNOWLOGINWINDOW_H
#define SNOWLOGINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>

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
    void onRequestCaptchaImageFinished();
    void onLoginFinished();

private:
    void startRequestingCaptchaImage();
    void startSendingLoginRequest();
    void displayAiocpWebView();

    Ui::SnowLoginWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *captchaImageReply;
    QNetworkReply *loginReply;
    QList<QNetworkCookie> cookiesForAIOCP;
    QString captchaCode;
};
#endif // SNOWLOGINWINDOW_H
