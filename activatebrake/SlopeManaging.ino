#define UPPER_THRESHOLD 15 // Need Test on the real Slope 
#define LOWER_THRESHOLD -15 // Need Test on the real Slope 
#define Adjusment 2 // Need Test on the real Slope 
static bool brakeFlag = false; 

void brakingSystem(int16_t slopeValue) {
    
    if (slopeValue > UPPER_THRESHOLD + Adjusment) {
        if (!brakeFlag) {
            activateBrake_1();
            activateBrake_2();
        }
    } else if (slopeValue < LOWER_THRESHOLD - Adjusment) {
        if (!brakeFlag) {
            activateBrake_1();
            activateBrake_2();
        }
    } else if (slopeValue < UPPER_THRESHOLD - Adjusment || slopeValue > LOWER_THRESHOLD + Adjusment) {
        if (brakeFlag) {
            return_brake();
            brakeFlag = false; 
    }
}
