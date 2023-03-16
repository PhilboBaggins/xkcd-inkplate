
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include <Arduino_JSON.h>
#include "wifi-credentials.h"

Inkplate display(INKPLATE_3BIT);

const char* const LATEST_XKCD_URL = "https://xkcd.com/info.0.json";

char HTTP_PAYLOAD[4 * 1024]; // TODO: Size???????

char XKCD_URL[29 + 10 + 1];
const char* const getXkcdUrl(int comicId)
{
    sprintf(XKCD_URL, "https://xkcd.com/%d/info.0.json", comicId);
    return XKCD_URL;
}

bool getHttpBody(const char* const url, char* payload, size_t payloadLen)
{
    HTTPClient http;
    if (http.begin(url) && (http.GET() == HTTP_CODE_OK))
    {
        strncpy(payload, http.getString().c_str(), payloadLen);
        http.end();
        return true;
    }
    else
    {
        Serial.println("Fail");
        http.end();
        return false;
    }
}

bool getXkcdLatestNumber(int* latestXkcdNum)
{
    if (getHttpBody(LATEST_XKCD_URL, HTTP_PAYLOAD, sizeof(HTTP_PAYLOAD)))
    {
        JSONVar jsonData = JSON.parse(HTTP_PAYLOAD);
        *latestXkcdNum = (int)jsonData["num"];
        return true;
    }
    else
    {
        *latestXkcdNum = 0;
        return false;
    }
}

void getComic(int comicId)
{
    const char* const xkcdUrl = getXkcdUrl(comicId);
    if (getHttpBody(xkcdUrl, HTTP_PAYLOAD, sizeof(HTTP_PAYLOAD)))
    {
        JSONVar jsonData = JSON.parse(HTTP_PAYLOAD);
        const char* const title     = (const char* const)jsonData["title"]; // TODO: Should I grab jsonData["safe_title"] instead?
        const char* const imgageURL = (const char* const)jsonData["img"];
        const char* const altText   = (const char* const)jsonData["altText"];

        Serial.println(HTTP_PAYLOAD);
        Serial.print(comicId);
        Serial.print(" - ");
        Serial.println(title);
        Serial.println();

        display.clearDisplay();
        display.drawImage(imgageURL, display.PNG, 0, 0);
        display.display();
    }
}

void tryDisplayComic()
{
    int latestXkcdNum;
    if (!getXkcdLatestNumber(&latestXkcdNum))
        return;

    const int randomComic = random(9, latestXkcdNum + 1);
    getComic(randomComic);
}

void setup()
{
    Serial.begin(115200);
    display.begin();

    display.joinAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop()
{
    //Serial.println("Attemping download");
    tryDisplayComic();
        
    //delay(60 * 1000); // 1 minute
    delay(5 * 1000); // 5 seconds
}
