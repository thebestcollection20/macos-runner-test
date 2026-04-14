#include <QApplication>
#include "mainwindow.h"
#include "ui/theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("CaptionStudio");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CaptionStudio");

    // Apply DaVinci Resolve-inspired dark theme
    DaVinciTheme::apply(&app);

    MainWindow window;
    window.show();

    return app.exec();
}
