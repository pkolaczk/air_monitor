
#include <AceTime.h>

#include "Publisher.h"


const char* KEYSPACE = "public";
const char* TABLE = "measurements";
const char* SENSOR_ID = "81fc94c8-dc84-49bf-9d82-df629b8555c2";

const acetime_t PUBLISH_INTERVAL_SECONDS = 60;

    
Publisher::Publisher(ThSensor& th, PmSensor& pm, Clock& clock)
        : client(AstraClient()), th(th), pm(pm), clock(clock), lastPublishTime(0) {
}

void Publisher::init() {
    fs::File secret = SPIFFS.open("/secret/astra.txt", "r");
    String dbId = secret.readStringUntil('\n');
    String dbRegion = secret.readStringUntil('\n');
    String dbUser = secret.readStringUntil('\n');
    String dbPassword = secret.readStringUntil('\n');
    secret.close();


    Serial.println("Connecting to Astra...");    
    client.logger->setLevel(LEVEL_DEBUG);
    int status = client.connect(
        // client.connect() stores string references in the client object, so we must duplicate them.
        // strdup memory leak here, but we do it only once for the lifetime of the program, so nobody cares ;)
        strdup(dbId.c_str()),                
        strdup(dbRegion.c_str()), 
        strdup(dbUser.c_str()), 
        strdup(dbPassword.c_str()));
    if (status == 0) 
        Serial.println("Connected to Astra!");    
    else 
        Serial.println("Failed to connect, return code = " + status);
}

void Publisher::loop() {
    if ((clock.getNow() - lastPublishTime > PUBLISH_INTERVAL_SECONDS) && pm.isReady() && th.isReady()) {
        publish();
    }
}

void Publisher::read(Measurement& meas) {
    acetime_t time = clock.getNow();
    LocalDateTime ldt = LocalDateTime::forEpochSeconds(time);
    snprintf(meas.day, sizeof(meas.day), "%04d-%02d-%02d", 
        ldt.year(), ldt.month(), ldt.day());
    snprintf(meas.timestamp, sizeof(meas.timestamp), "%04d-%02d-%02dT%02d:%02d:%02dZ", 
        ldt.year(), ldt.month(), ldt.day(), ldt.hour(), ldt.minute(), ldt.second());
    snprintf(meas.temperature, sizeof(meas.temperature), "%3.1f", th.getTemperature());
    snprintf(meas.humidity, sizeof(meas.humidity), "%3.1f", th.getHumidity());
    snprintf(meas.pm1, sizeof(meas.pm1), "%d", pm.getPm1());
    snprintf(meas.pm2_5, sizeof(meas.pm2_5), "%d", pm.getPm2_5());
    snprintf(meas.pm10, sizeof(meas.pm10), "%d", pm.getPm10());
}

void Publisher::publish() {
    lastPublishTime = clock.getNow();
    Serial.println("Publishing sensor readouts to Astra...");

    Measurement meas;
    read(meas);
    
    struct AstraClient::KeyVal columns[10] = { 
        { "sensor_id", SENSOR_ID }, 
        { "day", meas.day }, 
        { "ts", meas.timestamp }, 
        { "temp", meas.temperature }, 
        { "humidity", meas.humidity }, 
        { "pm01", meas.pm1 }, 
        { "pm02", meas.pm2_5 }, 
        { "pm10", meas.pm10 },
        { "latitude", "52.237049" },
        { "longitude", "21.017532" } 
    };
    client.addRow(KEYSPACE, TABLE, 10, columns);
}