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
    Publisher(ThSensor& th, PmSensor& pm, Clock& clock);
    
    // Connects to Astra
    void init();

    // Must be called in the main loop of the program, 
    // will periodically publish sensor readouts
    void loop();

    // Reads sensor values and sends them to Astra database
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





