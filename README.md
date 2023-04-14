 Health Centre Records System with Pro Mini 2560, ESP32, TFT, Fingerprint Module, and Buzzer

Introduction:
This project aims to provide a comprehensive solution for health centers to manage their patient records and access control. The system is built using a combination of Pro Mini 2560, ESP32, TFT, Fingerprint Module, and Buzzer. The Pro Mini 2560 acts as the main controller, while the ESP32 is used to send data to the API for the system.

System Flow:
The system follows a step-by-step process as outlined below:

Fingerprint Registration:
The patient is then required to register their fingerprint using the fingerprint module. This is done by placing their finger on the module for a few seconds until the system records their fingerprint.

Access Control:
Once the patient has registered their details and fingerprint, they can use the system to access the health center. To gain access, the patient places their finger on the fingerprint module, and the system compares the fingerprint with the one on record. If the fingerprints match, the system grants access, and the buzzer sounds to alert the patient.

Patient Check-In:
After gaining access, the patient can then check-in by selecting their appointment time and the doctor they wish to see. 

Data Transfer:
The system then uses the ESP32 to send the patient's information to the API for the system. This information is stored in the system's database for future reference.

Conclusion:
This project provides an efficient and effective solution for managing patient records and access control in health centers. With its easy-to-use interface and reliable technology, it is sure to improve the overall experience for both patients and health center staff.
