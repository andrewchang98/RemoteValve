#define DRVIN1 3
#define DRVIN2 4
#define DRVSLP 2
#define V5_EN 6
#define PRES_IN 18
#define SOLAR_IN 19

String SENSOR = "PX3";

const double pressureLimit = 46.0;

String position = "n/a";
String pressure = "n/a";
String charge = "n/a";
String solar = "n/a";
String version = "v0.060422";

FuelGauge battery;

void setup() {
    Particle.publishVitals(3600);   //hourly
    Particle.keepAlive(600);        //ping every 10 minutes
    
    Particle.function("Terminal", terminal);
    
    Particle.variable("1-Position", position);
    Particle.variable("2-Pressure", pressure);
    Particle.variable("3-Battery", charge);
    Particle.variable("4-Solar", solar);
    Particle.variable("5-Version", version);
    Particle.variable("6-Sensor", SENSOR)
    
    pinMode(DRVIN1, OUTPUT);
    pinMode(DRVIN2, OUTPUT);
    pinMode(DRVSLP, OUTPUT);
    pinMode(V5_EN, OUTPUT);
    pinMode(PRES_IN, INPUT);
    pinMode(SOLAR_IN, INPUT);
}

void loop() {
    if(!Particle.connected() && System.millis() > 600000) {
        System.reset();
    }
    checkBat();
    checkPres(pressureLimit);
}

int terminal(String cmd) {
    if(cmd == "close") {
        closeValve(100);
        return 2;
    }
    if(cmd == "open") {
        openValve(100);
        return 1;
    }
    return -1;
}

void closeValve(int t) {
    digitalWrite(DRVSLP, HIGH);
    digitalWrite(DRVIN1, HIGH);
    digitalWrite(DRVIN2, LOW);
    delay(t);
    digitalWrite(DRVSLP, LOW);
    digitalWrite(DRVIN1, LOW);
    digitalWrite(DRVIN2, LOW);
    position = "closed";
}

void openValve(int t) {
    digitalWrite(DRVSLP, HIGH);
    digitalWrite(DRVIN1, LOW);
    digitalWrite(DRVIN2, HIGH);
    delay(t);
    digitalWrite(DRVSLP, LOW);
    digitalWrite(DRVIN1, LOW);
    digitalWrite(DRVIN2, LOW);
    position = "open";
}

void checkBat() {
    double dQ = battery.getNormalizedSoC();
    double dS = analogRead(SOLAR_IN) * 0.008072; //0.0008 * 10.09
    String sQ = String(dQ, 0);
    String sS = String(dS, 1);
    charge = String(sQ + "%");
    solar = String(sS + "V");
}

void checkPres(double limit) {
    double p = readPressure();
    if(p > limit) {
        openValve(3000);
    }
    String sP = String(p, 0);
    pressure = String(sP + "psi");
}

double readPressure() {
    digitalWrite(V5_EN, HIGH);
    delay(25);
    double rawP = analogRead(PRES_IN) * 0.0012; //0.0012 = 0.0008mV * 1.5
    digitalWrite(V5_EN, LOW);
    if(SENSOR == "PX3") {
        return 25 * (rawP - 0.5) - 14;
    }
    if(SENSOR == "MLH") {
        return 37.5 * (rawP - 0.5);
    }
 }
