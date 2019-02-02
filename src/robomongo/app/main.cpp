#include <QApplication>
#include <QDesktopWidget>

#include <locale.h>

// Header "mongo/util/net/sock" is needed for mongo::enableIPv6()
// Header "mongo/platform/basic" is required by "sock.h" under Windows
#include <mongo/platform/basic.h>
#include <mongo/util/net/sock.h>
#include <mongo/base/initializer.h>
#include <mongo/util/net/ssl_options.h>

#include "robomongo/core/AppRegistry.h"
#include "robomongo/core/settings/SettingsManager.h"
#include "robomongo/gui/MainWindow.h"
#include "robomongo/gui/AppStyle.h"
#include "robomongo/gui/dialogs/EulaDialog.h"
#include "robomongo/ssh/ssh.h"


int main(int argc, char *argv[], char** envp)
{
    if (rbm_ssh_init())
        return 1;

    // Please check, do we really need envp for other OSes?
#ifdef Q_OS_WIN
    envp = NULL;
#endif

    // Support for IPv6 is disabled by default. Enable it.
    mongo::enableIPv6(true);

    // Perform SSL-enabled mongo initialization
    mongo::sslGlobalParams.sslMode.store(mongo::SSLParams::SSLMode_allowSSL);

    std::string arg1 = argc > 1 ? argv[1] : "";
    // Cross Platform High DPI support - Qt 5.7
    if ("no-hdpi-scaling" == arg1) {
        std::cout << "HDPI Scaling disabled" << std::endl;
    } else {
        std::cout << "HDPI Scaling enabled" << std::endl;
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }


    // Initialization routine for MongoDB shell
    mongo::runGlobalInitializersOrDie(argc, argv, envp);

    // Initialize Qt application
    QApplication app(argc, argv);

    // On Unix/Linux Qt is configured to use the system locale settings by default.
    // This can cause a conflict when using POSIX functions, for instance, when
    // converting between data types such as floats and strings, since the notation
    // may differ between locales. To get around this problem, call the POSIX
    // function setlocale(LC_NUMERIC, "C") right after initializing QApplication or
    // QCoreApplication to reset the locale that is used for number formatting to "C"-locale.
    // (http://doc.qt.io/qt-5/qcoreapplication.html#locale-settings)
    setlocale(LC_NUMERIC, "C");

    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef Q_OS_MAC
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    auto const &settingsManager = Robomongo::AppRegistry::instance().settingsManager();
    // Disable HTTPS and do not show EULA form if program exited abnormally
    bool showEulaDialogForm = true;
    if (!settingsManager->programExitedNormally()) {
        showEulaDialogForm = false;
        settingsManager->setUseHttps(false);
        settingsManager->save();
    }

    if (!settingsManager->useHttps())
        showEulaDialogForm = false;

    // EULA License Agreement
    if (!settingsManager->acceptedEulaVersions().contains(PROJECT_VERSION)) {
        Robomongo::EulaDialog eulaDialog(showEulaDialogForm);
        settingsManager->setProgramExitedNormally(false);
        settingsManager->save();
        int const result = eulaDialog.exec();
        settingsManager->setProgramExitedNormally(true);
        settingsManager->save();
        if (QDialog::Rejected == result) {
            rbm_ssh_cleanup();
            return 1;
        }
        // EULA accepted
        settingsManager->addAcceptedEulaVersion(PROJECT_VERSION);
        settingsManager->save();
    }

    // Init GUI style
    Robomongo::AppStyleUtils::initStyle();

    // To be set true at normal program exit
    settingsManager->setProgramExitedNormally(false);
    settingsManager->save();

    // Application main window
    Robomongo::MainWindow mainWindow;
    mainWindow.show();

    int rc = app.exec();
    rbm_ssh_cleanup();
    return rc;
}
