/**
 * @file PageProvider.h
 * @author Tim Dietrich, Felix Herrling, Niclas Jost, Marius Busalt
 * @brief This component implements an abstract class as foundation for the different pages of the webserver.
 *        Each page has to implement the handler method to provide the html content.
 * @version 2.0
 * @date 2026-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PAGEPROVIDER_H
#define PAGEPROVIDER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <Arduino.h>

class PageProvider
{
public:
    virtual ~PageProvider() = default;

    /**
     * @brief provides html on initial request from client
     * @return void
     */
    virtual void handler() = 0;

    static void serveFileFromSpiffs(WebServer *server, const char *filename, const char *contentType);
};
#endif //PAGEPROVIDER_H
