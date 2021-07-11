#include "snowloginwindow.h"

#include <QApplication>
#include <QNetworkProxy>

void setApplicationProxy(QApplication &app)
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName("192.168.254.12");
    proxy.setPort(1080);
//    proxy.setUser("username");
//    proxy.setPassword("password");
    QNetworkProxy::setApplicationProxy(proxy);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("YongzhouTelecom-AIOCP");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationDomain("xuefeng.space");
    setApplicationProxy(app);

    SnowLoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
