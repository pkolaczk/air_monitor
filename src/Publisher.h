#include <Astra.h>

#include "PmSensor.h"
#include "ThSensor.h"

// A structure storing text data to be send to Astra.
// Astra REST API accepts only textual data (even numbers must be converted to text).
struct Measurement {
    char day[12];
    char timestamp[32];
    char temperature[7];
    char humidity[7];
    char pm10[5];
    char pm2_5[5];
    char pm1[5];
};


// Publishes sensor readouts to Astra database
class Publisher {
public:
    // Creates a new publisher that will read the temperature and humidity
    // from the `th` object, air pollution information from the `pm` sensor object,
    // and current NTP-synchronized timestamp from `clock`.
    Publisher(ThSensor& th, PmSensor& pm, Clock& clock);
    
    // Connects to Astra.
    // The credentials needs to be placed in the SPIFFS filesystem in 
    // file /data/secret/astra.txt. The file should consist of 4 lines:
    // 1. database uuid
    // 2. region 
    // 3. username (accound identifier)
    // 4. password (Astra token)
    void init();

    // Must be called in the main loop of the program, 
    // will publish sensor readouts, every 10-minutes.
    // It checks if the sensors are ready by calling `isReady()` on them
    // before fetching the data.
    void loop();

    // Reads sensor values and sends them to Astra database.
    void publish();


private:
    AstraClient client;
    ThSensor& th;
    PmSensor& pm;
    Clock& clock;
    acetime_t lastPublishTime;

    // Reads clock and sensor values into meas struct
    void read(Measurement& meas);
};





