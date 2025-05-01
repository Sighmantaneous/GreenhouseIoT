
             ___  ___  _____   ____   __  __     _     ____  _____ 
            |_ _|/ _ \|_   _| / ___| |  \/  |   / \   |  _ \|_   _|
             | || | | | | |   \___ \ | |\/| |  / _ \  | |_) | | |  
             | || |_| | | |    ___) || |  | | / ___ \ |  _ <  | |  
            |___|\___/  |_|   |____/ |_|  |_|/_/   \_\|_| \_\ |_|  
                                                                   
                    ____     _     ____   ____   _____  _   _ 
                   / ___|   / \   |  _ \ |  _ \ | ____|| \ | |
                  | |  _   / _ \  | |_) || | | ||  _|  |  \| |
                  | |_| | / ___ \ |  _ < | |_| || |___ | |\  |
                   \____|/_/   \_\|_| \_\|____/ |_____||_| \_|
                                                              
     _   _  _____  ____   ____         ___  _   _  ____  ___  ____   _____ 
    | | | || ____||  _ \ | __ )       |_ _|| \ | |/ ___||_ _||  _ \ | ____|
    | |_| ||  _|  | |_) ||  _ \  _____ | | |  \| |\___ \ | | | | | ||  _|  
    |  _  || |___ |  _ < | |_) ||_____|| | | |\  | ___) || | | |_| || |___ 
    |_| |_||_____||_| \_\|____/       |___||_| \_||____/|___||____/ |_____|
     






























Sensors + Hardware used

- ESP32:
    The main micro-controller powering this project

- Temperature + humity sensor:
    The AHT20 provides both temp and humidy readings via an I²C interface on single sensor.

- Peristaltic Pump:
    Used to irrigate soil if moisture level drops below certain value (25% in this case)

- DC fan(x2):
    Used to help remove build up of heat and circulate fresher air in the greenhouse, when window is open. 

- 2 channel power relay :
    Used with both fans to regulate voltage and prevent current/ voltage spikes damaging componants

- Soil moisture sensor :
    The soil moisture sensor consists of two probes that allow the current to pass through the soil and then obtain resistance
    values to measure soil moisture content.

- Servo Motor : 
    FS90 is a small servo motor that uses PWM(pusle wave modulation) that is capable of 180° rotation. Used here to open/close window 



