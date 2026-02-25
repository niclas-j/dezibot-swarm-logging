/**
 * @file MainPage.cpp
 * @author Tim Dietrich, Felix Herrling
 * @brief Implementation of the MainPage class.
 * @version 1.0
 * @date 2025-03-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "MainPage.h"

MainPage::MainPage(WebServer* server):serverPointer(server) {}

void MainPage::handler() {
    serveFileFromSpiffs(serverPointer, "/index.html", "text/html");
};
