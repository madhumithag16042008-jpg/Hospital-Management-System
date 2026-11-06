/*
 * =====================================================================================
 * Project Title   : Hospital Management System
 * Language        : C++ (C++17 standard compatible)
 * Application Type: Console-Based Application
 * 
 * Key Object-Oriented Programming (OOP) Concepts Demonstrated:
 *   1. Classes and Objects   : Representation of real-world entities (Person, Patient, 
 *                              InPatient, OutPatient, HospitalSystem).
 *   2. Encapsulation        : Private/Protected data members with public getters/setters.
 *   3. Inheritance          : Multi-level and hierarchical inheritance:
 *                              Person -> Patient -> InPatient / OutPatient.
 *   4. Polymorphism         : Virtual functions (calculateBill, displayDetails, serialize)
 *                              executed dynamically via base class pointers.
 *   5. Abstract Base Class  : Person and Patient declare pure virtual functions (= 0),
 *                              enforcing implementation in derived classes.
 * 
 * Additional Features & Best Practices:
 *   - STL Vector for dynamic in-memory record storage.
 *   - File Handling (<fstream>) to persist and retrieve records across sessions.
 *   - Exception Handling (try-catch with custom HospitalException class).
 *   - Robust Input Validation (prevents cin crashes on invalid types, validates ranges).
 *   - Clean, beginner-friendly menu-driven interface with itemized bill generation.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <exception>
#include <memory>
#include <cctype>

using namespace std;

// =====================================================================================
// Custom Exception Class for Hospital Management Operations
// =====================================================================================
class HospitalException : public exception {
private:
    string errorMessage;
public:
    explicit HospitalException(const string& msg) : errorMessage(msg) {}

    // Override what() method to return custom error message
    const char* what() const noexcept override {
        return errorMessage.c_str();
    }
};

// =====================================================================================
// Utility / Input Validation Functions
// =====================================================================================

// Clears remaining characters from the input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Safely reads a non-empty string line from standard input
string getValidString(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);

        // Trim leading and trailing spaces
        size_t first = input.find_first_not_of(" \t\r\n");
        if (first == string::npos) {
            cout << "  [!] Input cannot be empty. Please enter a valid text.\n";
            continue;
        }
        size_t last = input.find_last_not_of(" \t\r\n");
        input = input.substr(first, (last - first + 1));
        return input;
    }
}

// Validates and reads an integer within a specified range
int getValidInt(const string& prompt, int minVal = 0, int maxVal = 1000000000) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal && value <= maxVal) {
                clearInputBuffer();
                return value;
            } else {
                cout << "  [!] Value out of allowed range (" << minVal << " to " << maxVal << "). Try again.\n";
            }
        } else {
            cout << "  [!] Invalid input! Please enter a valid number.\n";
            clearInputBuffer();
        }
    }
}

// Validates and reads a floating point (double) value >= minVal
double getValidDouble(const string& prompt, double minVal = 0.0) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal) {
                clearInputBuffer();
                return value;
            } else {
                cout << "  [!] Value must be greater than or equal to " << minVal << ". Try again.\n";
            }
        } else {
            cout << "  [!] Invalid input! Please enter a valid numeric amount.\n";
            clearInputBuffer();
        }
    }
}

// Validates phone number input (digits only, length between 7 and 15)
string getValidPhone(const string& prompt) {
    while (true) {
        string phone = getValidString(prompt);
        bool valid = true;
        if (phone.length() < 7 || phone.length() > 15) {
            valid = false;
        } else {
            for (char ch : phone) {
                if (!isdigit(static_cast<unsigned char>(ch)) && ch != '+' && ch != '-') {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {
            return phone;
        } else {
            cout << "  [!] Invalid contact number! Please enter 7-15 digits (e.g. 9876543210).\n";
        }
    }
}

// =====================================================================================
// OOP CONCEPT: Abstract Base Class - Person
// Demonstrates Encapsulation & Abstract Base Class
// =====================================================================================
class Person {
protected:
    // Encapsulated data members accessible to derived classes
    string name;
    int age;
    string gender;
    string contactNumber;

public:
    // Parameterized Constructor
    Person(const string& pName, int pAge, const string& pGender, const string& pContact)
        : name(pName), age(pAge), gender(pGender), contactNumber(pContact) {}

    // Virtual Destructor (Crucial for safe polymorphic deletion)
    virtual ~Person() {}

    // Getter and Setter Methods (Encapsulation)
    string getName() const { return name; }
    void setName(const string& pName) { name = pName; }

    int getAge() const { return age; }
    void setAge(int pAge) { age = pAge; }

    string getGender() const { return gender; }
    void setGender(const string& pGender) { gender = pGender; }

    string getContactNumber() const { return contactNumber; }
    void setContactNumber(const string& pContact) { contactNumber = pContact; }

    // Pure Virtual Function: makes Person an Abstract Base Class
    virtual void displayDetails() const = 0;
};

// =====================================================================================
// OOP CONCEPT: Inheritance & Abstract Class - Patient (Derived from Person)
// Demonstrates Multi-level Inheritance and Polymorphism
// =====================================================================================
class Patient : public Person {
protected:
    int patientId;
    string disease;
    string assignedDoctor;

public:
    // Constructor delegating common attributes to base class Person
    Patient(int id, const string& pName, int pAge, const string& pGender,
            const string& pContact, const string& pDisease, const string& pDoctor)
        : Person(pName, pAge, pGender, pContact),
          patientId(id), disease(pDisease), assignedDoctor(pDoctor) {}

    virtual ~Patient() {}

    // Getters and Setters
    int getPatientId() const { return patientId; }
    void setPatientId(int id) { patientId = id; }

    string getDisease() const { return disease; }
    void setDisease(const string& pDisease) { disease = pDisease; }

    string getAssignedDoctor() const { return assignedDoctor; }
    void setAssignedDoctor(const string& pDoctor) { assignedDoctor = pDoctor; }

    // Pure Virtual Functions to be overridden by concrete patient types
    virtual double calculateBill() const = 0;
    virtual string getPatientType() const = 0;
    virtual string serialize() const = 0;
};

// =====================================================================================
// OOP CONCEPT: Concrete Derived Class - InPatient (Admitted Patient)
// Demonstrates Polymorphism and Method Overriding
// =====================================================================================
class InPatient : public Patient {
private:
    int daysAdmitted;
    double dailyRoomCharge;
    double doctorFee;
    double medicineCharges;

public:
    InPatient(int id, const string& pName, int pAge, const string& pGender,
              const string& pContact, const string& pDisease, const string& pDoctor,
              int days, double roomCharge, double docFee, double medCharges)
        : Patient(id, pName, pAge, pGender, pContact, pDisease, pDoctor),
          daysAdmitted(days), dailyRoomCharge(roomCharge),
          doctorFee(docFee), medicineCharges(medCharges) {}

    // Getters and Setters
    int getDaysAdmitted() const { return daysAdmitted; }
    void setDaysAdmitted(int days) { daysAdmitted = days; }

    double getDailyRoomCharge() const { return dailyRoomCharge; }
    void setDailyRoomCharge(double charge) { dailyRoomCharge = charge; }

    double getDoctorFee() const { return doctorFee; }
    void setDoctorFee(double fee) { doctorFee = fee; }

    double getMedicineCharges() const { return medicineCharges; }
    void setMedicineCharges(double charges) { medicineCharges = charges; }

    // Polymorphic implementation of Bill Calculation
    // Total Bill = (Days * Room Charge) + Doctor Fee + Medicine Charges
    double calculateBill() const override {
        return (daysAdmitted * dailyRoomCharge) + doctorFee + medicineCharges;
    }

    string getPatientType() const override {
        return "In-Patient (Admitted)";
    }

    // Display formatted In-Patient details
    void displayDetails() const override {
        cout << left;
        cout << "\n============================================================\n";
        cout << "                  IN-PATIENT RECORD DETAILS                 \n";
        cout << "============================================================\n";
        cout << setw(22) << " Patient ID:"        << patientId << "\n";
        cout << setw(22) << " Patient Name:"      << name << "\n";
        cout << setw(22) << " Age / Gender:"      << age << " yrs / " << gender << "\n";
        cout << setw(22) << " Contact Number:"   << contactNumber << "\n";
        cout << setw(22) << " Diagnosis:"         << disease << "\n";
        cout << setw(22) << " Assigned Doctor:"   << assignedDoctor << "\n";
        cout << setw(22) << " Patient Category:"  << getPatientType() << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(22) << " Days Admitted:"     << daysAdmitted << " day(s)\n";
        cout << setw(22) << " Room Charge / Day:" << "Rs. " << fixed << setprecision(2) << dailyRoomCharge << "\n";
        cout << setw(22) << " Doctor Fee:"        << "Rs. " << doctorFee << "\n";
        cout << setw(22) << " Medicine Charges:"  << "Rs. " << medicineCharges << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(22) << " TOTAL BILL AMOUNT:" << "Rs. " << calculateBill() << "\n";
        cout << "============================================================\n";
    }

    // Format record into a single delimited line for file storage
    // Format: IN|id|name|age|gender|contact|disease|doctor|days|dailyCharge|docFee|medCharges
    string serialize() const override {
        ostringstream oss;
        oss << "IN|"
            << patientId << "|"
            << name << "|"
            << age << "|"
            << gender << "|"
            << contactNumber << "|"
            << disease << "|"
            << assignedDoctor << "|"
            << daysAdmitted << "|"
            << fixed << setprecision(2) << dailyRoomCharge << "|"
            << doctorFee << "|"
            << medicineCharges;
        return oss.str();
    }
};

// =====================================================================================
// OOP CONCEPT: Concrete Derived Class - OutPatient (OPD / Consultation)
// Demonstrates Polymorphism and Method Overriding
// =====================================================================================
class OutPatient : public Patient {
private:
    double consultationFee;
    double medicineCharges;

public:
    OutPatient(int id, const string& pName, int pAge, const string& pGender,
               const string& pContact, const string& pDisease, const string& pDoctor,
               double consultFee, double medCharges)
        : Patient(id, pName, pAge, pGender, pContact, pDisease, pDoctor),
          consultationFee(consultFee), medicineCharges(medCharges) {}

    // Getters and Setters
    double getConsultationFee() const { return consultationFee; }
    void setConsultationFee(double fee) { consultationFee = fee; }

    double getMedicineCharges() const { return medicineCharges; }
    void setMedicineCharges(double charges) { medicineCharges = charges; }

    // Polymorphic implementation of Bill Calculation
    // Total Bill = Consultation Fee + Medicine Charges
    double calculateBill() const override {
        return consultationFee + medicineCharges;
    }

    string getPatientType() const override {
        return "Out-Patient (OPD)";
    }

    // Display formatted Out-Patient details
    void displayDetails() const override {
        cout << left;
        cout << "\n============================================================\n";
        cout << "                 OUT-PATIENT RECORD DETAILS                 \n";
        cout << "============================================================\n";
        cout << setw(22) << " Patient ID:"        << patientId << "\n";
        cout << setw(22) << " Patient Name:"      << name << "\n";
        cout << setw(22) << " Age / Gender:"      << age << " yrs / " << gender << "\n";
        cout << setw(22) << " Contact Number:"   << contactNumber << "\n";
        cout << setw(22) << " Diagnosis:"         << disease << "\n";
        cout << setw(22) << " Assigned Doctor:"   << assignedDoctor << "\n";
        cout << setw(22) << " Patient Category:"  << getPatientType() << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(22) << " Consultation Fee:"  << "Rs. " << fixed << setprecision(2) << consultationFee << "\n";
        cout << setw(22) << " Medicine Charges:"  << "Rs. " << medicineCharges << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(22) << " TOTAL BILL AMOUNT:" << "Rs. " << calculateBill() << "\n";
        cout << "============================================================\n";
    }

    // Format record into a single delimited line for file storage
    // Format: OUT|id|name|age|gender|contact|disease|doctor|consultFee|medCharges
    string serialize() const override {
        ostringstream oss;
        oss << "OUT|"
            << patientId << "|"
            << name << "|"
            << age << "|"
            << gender << "|"
            << contactNumber << "|"
            << disease << "|"
            << assignedDoctor << "|"
            << fixed << setprecision(2) << consultationFee << "|"
            << medicineCharges;
        return oss.str();
    }
};

// =====================================================================================
// Hospital Management Controller Class
// Manages patient vectors, file operations, searching, updating, and bill calculations
// =====================================================================================
class HospitalSystem {
private:
    vector<Patient*> patients; // Dynamic array of base class pointers (Polymorphic vector)
    const string dataFileName;

    // Helper: Finds index of a patient by ID; returns -1 if not found
    int findPatientIndex(int id) const {
        for (size_t i = 0; i < patients.size(); ++i) {
            if (patients[i]->getPatientId() == id) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

public:
    explicit HospitalSystem(const string& fileName = "patients_records.txt")
        : dataFileName(fileName) {
        // Automatically load existing records from file on startup
        loadRecordsFromFile();
    }

    // Destructor: Clean up dynamically allocated memory in vector to prevent memory leaks
    ~HospitalSystem() {
        for (Patient* p : patients) {
            delete p;
        }
        patients.clear();
    }

    // ---------------------------------------------------------------------------------
    // Feature 1: Add New Patient Details
    // ---------------------------------------------------------------------------------
    void addNewPatient() {
        cout << "\n============================================================\n";
        cout << "                     ADD NEW PATIENT                        \n";
        cout << "============================================================\n";

        int id = getValidInt("Enter Patient ID (e.g. 101): ", 1, 999999);

        // Validation: Verify if Patient ID is unique
        if (findPatientIndex(id) != -1) {
            throw HospitalException("Patient with ID " + to_string(id) + " already exists!");
        }

        string name = getValidString("Enter Patient Full Name: ");
        int age = getValidInt("Enter Age (1 - 120): ", 1, 120);

        cout << "Select Gender:\n  1. Male\n  2. Female\n  3. Other\n";
        int genderChoice = getValidInt("Choice (1-3): ", 1, 3);
        string gender = (genderChoice == 1) ? "Male" : (genderChoice == 2 ? "Female" : "Other");

        string contact = getValidPhone("Enter Contact Number: ");
        string disease = getValidString("Enter Diagnosis / Disease: ");
        string doctor  = getValidString("Enter Assigned Doctor's Name: ");

        cout << "\nSelect Patient Type:\n";
        cout << "  1. In-Patient  (Admitted in Hospital Ward/Room)\n";
        cout << "  2. Out-Patient (OPD / Consultation Only)\n";
        int typeChoice = getValidInt("Enter Type Choice (1 or 2): ", 1, 2);

        if (typeChoice == 1) {
            int days = getValidInt("Enter Number of Days Admitted: ", 1, 365);
            double dailyRate = getValidDouble("Enter Room Charge per Day (Rs.): ", 0.0);
            double docFee = getValidDouble("Enter Doctor Fee (Rs.): ", 0.0);
            double medCharges = getValidDouble("Enter Medicine / Test Charges (Rs.): ", 0.0);

            // Dynamically allocate InPatient object
            patients.push_back(new InPatient(id, name, age, gender, contact, disease, doctor,
                                             days, dailyRate, docFee, medCharges));
        } else {
            double consultFee = getValidDouble("Enter Doctor Consultation Fee (Rs.): ", 0.0);
            double medCharges = getValidDouble("Enter Medicine / Pharmacy Charges (Rs.): ", 0.0);

            // Dynamically allocate OutPatient object
            patients.push_back(new OutPatient(id, name, age, gender, contact, disease, doctor,
                                              consultFee, medCharges));
        }

        // Save immediately to ensure data persistence
        saveRecordsToFile();
        cout << "\n>>> Success: Patient record registered and saved successfully! <<<\n";
    }

    // ---------------------------------------------------------------------------------
    // Feature 2: Display All Patient Details
    // ---------------------------------------------------------------------------------
    void displayAllPatients() const {
        if (patients.empty()) {
            throw HospitalException("No patient records found in the system.");
        }

        cout << "\n========================================================================================\n";
        cout << "                                  ALL PATIENT RECORDS                                   \n";
        cout << "========================================================================================\n";
        cout << left 
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(6)  << "Age"
             << setw(8)  << "Gender"
             << setw(16) << "Category"
             << setw(18) << "Diagnosis"
             << setw(12) << "Bill (Rs.)"
             << "\n";
        cout << "----------------------------------------------------------------------------------------\n";

        for (const Patient* p : patients) {
            cout << left 
                 << setw(6)  << p->getPatientId()
                 << setw(20) << (p->getName().length() > 18 ? p->getName().substr(0, 15) + "..." : p->getName())
                 << setw(6)  << p->getAge()
                 << setw(8)  << p->getGender()
                 << setw(16) << (p->getPatientType() == "In-Patient (Admitted)" ? "In-Patient" : "Out-Patient")
                 << setw(18) << (p->getDisease().length() > 16 ? p->getDisease().substr(0, 13) + "..." : p->getDisease())
                 << setw(12) << fixed << setprecision(2) << p->calculateBill()
                 << "\n";
        }
        cout << "========================================================================================\n";
        cout << "Total Registered Patients: " << patients.size() << "\n";
    }

    // ---------------------------------------------------------------------------------
    // Feature 3: Search Patients using Patient ID
    // ---------------------------------------------------------------------------------
    void searchPatientById() const {
        if (patients.empty()) {
            throw HospitalException("Hospital database is currently empty.");
        }

        int id = getValidInt("Enter Patient ID to Search: ", 1, 999999);
        int idx = findPatientIndex(id);

        if (idx == -1) {
            throw HospitalException("Patient with ID " + to_string(id) + " does not exist.");
        }

        cout << "\n>>> Record Found Successfully! <<<";
        // Polymorphic call: displays specialized details based on runtime object type
        patients[idx]->displayDetails();
    }

    // ---------------------------------------------------------------------------------
    // Feature 4: Update Patient Information
    // ---------------------------------------------------------------------------------
    void updatePatient() {
        if (patients.empty()) {
            throw HospitalException("Hospital database is empty. No records to update.");
        }

        int id = getValidInt("Enter Patient ID to Update: ", 1, 999999);
        int idx = findPatientIndex(id);

        if (idx == -1) {
            throw HospitalException("Cannot update. Patient ID " + to_string(id) + " not found.");
        }

        Patient* p = patients[idx];
        cout << "\nCurrent Details of Selected Patient:";
        p->displayDetails();

        cout << "\n--- Update Menu for Patient ID: " << id << " ---\n";
        cout << "  1. Update Contact Number\n";
        cout << "  2. Update Diagnosis / Disease\n";
        cout << "  3. Update Assigned Doctor\n";
        cout << "  4. Update Billing / Treatment Charges\n";
        cout << "  5. Update All Information\n";
        cout << "  0. Cancel Update\n";

        int subChoice = getValidInt("Choose field to update (0-5): ", 0, 5);
        if (subChoice == 0) {
            cout << "Update cancelled.\n";
            return;
        }

        if (subChoice == 1 || subChoice == 5) {
            string newPhone = getValidPhone("Enter New Contact Number: ");
            p->setContactNumber(newPhone);
        }

        if (subChoice == 2 || subChoice == 5) {
            string newDisease = getValidString("Enter Updated Diagnosis: ");
            p->setDisease(newDisease);
        }

        if (subChoice == 3 || subChoice == 5) {
            string newDoctor = getValidString("Enter New Assigned Doctor: ");
            p->setAssignedDoctor(newDoctor);
        }

        if (subChoice == 4 || subChoice == 5) {
            // Check dynamic type using dynamic_cast
            InPatient* inP = dynamic_cast<InPatient*>(p);
            if (inP != nullptr) {
                cout << "\nUpdating In-Patient Charges:\n";
                int days = getValidInt("Enter Number of Days Admitted: ", 1, 365);
                double dailyRate = getValidDouble("Enter Daily Room Charge (Rs.): ", 0.0);
                double docFee = getValidDouble("Enter Doctor Fee (Rs.): ", 0.0);
                double medFee = getValidDouble("Enter Medicine Charges (Rs.): ", 0.0);

                inP->setDaysAdmitted(days);
                inP->setDailyRoomCharge(dailyRate);
                inP->setDoctorFee(docFee);
                inP->setMedicineCharges(medFee);
            } else {
                OutPatient* outP = dynamic_cast<OutPatient*>(p);
                if (outP != nullptr) {
                    cout << "\nUpdating Out-Patient Charges:\n";
                    double consultFee = getValidDouble("Enter Consultation Fee (Rs.): ", 0.0);
                    double medFee = getValidDouble("Enter Medicine Charges (Rs.): ", 0.0);

                    outP->setConsultationFee(consultFee);
                    outP->setMedicineCharges(medFee);
                }
            }
        }

        // Save modifications to file
        saveRecordsToFile();
        cout << "\n>>> Success: Patient details updated and saved successfully! <<<\n";
    }

    // ---------------------------------------------------------------------------------
    // Feature 5: Delete Patient Records
    // ---------------------------------------------------------------------------------
    void deletePatient() {
        if (patients.empty()) {
            throw HospitalException("Hospital database is empty. No records to delete.");
        }

        int id = getValidInt("Enter Patient ID to Delete: ", 1, 999999);
        int idx = findPatientIndex(id);

        if (idx == -1) {
            throw HospitalException("Cannot delete. Patient ID " + to_string(id) + " not found.");
        }

        cout << "\nPatient Record to be Deleted:";
        patients[idx]->displayDetails();

        cout << "\nAre you sure you want to permanently delete this record? (Y/N): ";
        char confirm;
        cin >> confirm;
        clearInputBuffer();

        if (toupper(confirm) == 'Y') {
            // Free the dynamically allocated object
            delete patients[idx];
            // Erase the pointer from the vector
            patients.erase(patients.begin() + idx);
            // Save updated vector to disk
            saveRecordsToFile();
            cout << "\n>>> Success: Patient record ID " << id << " deleted successfully! <<<\n";
        } else {
            cout << "\nDeletion cancelled by user.\n";
        }
    }

    // ---------------------------------------------------------------------------------
    // Feature 6: Calculate Hospital Bills (Print Itemized Bill Invoice)
    // ---------------------------------------------------------------------------------
    void calculatePatientBill() const {
        if (patients.empty()) {
            throw HospitalException("No patient records available for billing.");
        }

        int id = getValidInt("Enter Patient ID to Generate Bill: ", 1, 999999);
        int idx = findPatientIndex(id);

        if (idx == -1) {
            throw HospitalException("Patient ID " + to_string(id) + " not found.");
        }

        const Patient* p = patients[idx];

        cout << left;
        cout << "\n============================================================\n";
        cout << "                  HOSPITAL BILLING INVOICE                  \n";
        cout << "============================================================\n";
        cout << setw(20) << " Invoice To:"      << p->getName() << "\n";
        cout << setw(20) << " Patient ID:"      << p->getPatientId() << "\n";
        cout << setw(20) << " Contact:"         << p->getContactNumber() << "\n";
        cout << setw(20) << " Doctor in Charge:" << p->getAssignedDoctor() << "\n";
        cout << setw(20) << " Care Type:"        << p->getPatientType() << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(35) << " Description" << right << setw(23) << "Amount (Rs.)" << "\n";
        cout << left;
        cout << "------------------------------------------------------------\n";

        const InPatient* inP = dynamic_cast<const InPatient*>(p);
        if (inP != nullptr) {
            double roomTotal = inP->getDaysAdmitted() * inP->getDailyRoomCharge();
            cout << setw(35) << (" Room Charges (" + to_string(inP->getDaysAdmitted()) + " days @ Rs. " + to_string(static_cast<int>(inP->getDailyRoomCharge())) + "/day)")
                 << right << setw(23) << fixed << setprecision(2) << roomTotal << "\n";
            cout << left << setw(35) << " Doctor / Consultation Fee"
                 << right << setw(23) << inP->getDoctorFee() << "\n";
            cout << left << setw(35) << " Medicine & Pharmacy Charges"
                 << right << setw(23) << inP->getMedicineCharges() << "\n";
        } else {
            const OutPatient* outP = dynamic_cast<const OutPatient*>(p);
            if (outP != nullptr) {
                cout << setw(35) << " OPD Consultation Fee"
                     << right << setw(23) << fixed << setprecision(2) << outP->getConsultationFee() << "\n";
                cout << left << setw(35) << " Medicine & Pharmacy Charges"
                     << right << setw(23) << outP->getMedicineCharges() << "\n";
            }
        }

        cout << "------------------------------------------------------------\n";
        cout << left  << setw(35) << " GRAND TOTAL" 
             << right << setw(23) << fixed << setprecision(2) << p->calculateBill() << "\n";
        cout << "============================================================\n";
        cout << "            Thank you! Wishing you a speedy recovery!       \n";
        cout << "============================================================\n";
    }

    // ---------------------------------------------------------------------------------
    // Additional Feature: Hospital Revenue & Statistics Summary
    // ---------------------------------------------------------------------------------
    void displaySummary() const {
        if (patients.empty()) {
            cout << "\n[i] No patient data currently available.\n";
            return;
        }

        int totalInPatients = 0;
        int totalOutPatients = 0;
        double totalRevenue = 0.0;

        for (const Patient* p : patients) {
            double bill = p->calculateBill();
            totalRevenue += bill;
            if (dynamic_cast<const InPatient*>(p) != nullptr) {
                totalInPatients++;
            } else {
                totalOutPatients++;
            }
        }

        cout << left;
        cout << "\n============================================================\n";
        cout << "               HOSPITAL REVENUE & STATISTICS                \n";
        cout << "============================================================\n";
        cout << setw(30) << " Total Registered Patients:" << patients.size() << "\n";
        cout << setw(30) << " Total Admitted In-Patients:" << totalInPatients << "\n";
        cout << setw(30) << " Total OPD Out-Patients:"    << totalOutPatients << "\n";
        cout << "------------------------------------------------------------\n";
        cout << setw(30) << " Total Calculated Revenue:"   << "Rs. " << fixed << setprecision(2) << totalRevenue << "\n";
        cout << "============================================================\n";
    }

    // ---------------------------------------------------------------------------------
    // Feature 7: Save Patient Records Using File Handling (<fstream>)
    // ---------------------------------------------------------------------------------
    void saveRecordsToFile() const {
        ofstream outFile(dataFileName, ios::out | ios::trunc);
        if (!outFile.is_open()) {
            throw HospitalException("Failed to open file for saving: " + dataFileName);
        }

        for (const Patient* p : patients) {
            outFile << p->serialize() << "\n";
        }

        outFile.close();
    }

    // ---------------------------------------------------------------------------------
    // Load Patient Records From File (<fstream>)
    // ---------------------------------------------------------------------------------
    void loadRecordsFromFile() {
        ifstream inFile(dataFileName);
        // If file doesn't exist yet, simply return without error (initial run)
        if (!inFile.is_open()) {
            return;
        }

        // Clean any existing memory before loading
        for (Patient* p : patients) {
            delete p;
        }
        patients.clear();

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string token;
            vector<string> fields;

            while (getline(ss, token, '|')) {
                fields.push_back(token);
            }

            try {
                if (fields.empty()) continue;

                if (fields[0] == "IN" && fields.size() >= 12) {
                    int id = stoi(fields[1]);
                    string name = fields[2];
                    int age = stoi(fields[3]);
                    string gender = fields[4];
                    string contact = fields[5];
                    string disease = fields[6];
                    string doctor = fields[7];
                    int days = stoi(fields[8]);
                    double dailyRate = stod(fields[9]);
                    double docFee = stod(fields[10]);
                    double medFee = stod(fields[11]);

                    patients.push_back(new InPatient(id, name, age, gender, contact,
                                                     disease, doctor, days, dailyRate, docFee, medFee));
                } else if (fields[0] == "OUT" && fields.size() >= 10) {
                    int id = stoi(fields[1]);
                    string name = fields[2];
                    int age = stoi(fields[3]);
                    string gender = fields[4];
                    string contact = fields[5];
                    string disease = fields[6];
                    string doctor = fields[7];
                    double consultFee = stod(fields[8]);
                    double medFee = stod(fields[9]);

                    patients.push_back(new OutPatient(id, name, age, gender, contact,
                                                      disease, doctor, consultFee, medFee));
                }
            } catch (...) {
                // Skip corrupted individual lines safely
                continue;
            }
        }

        inFile.close();
    }
};

// =====================================================================================
// Main Function: Console Menu Interface
// =====================================================================================
int main() {
    try {
        HospitalSystem hospital("patients_records.txt");
        int choice = 0;

        do {
            cout << "\n============================================================\n";
            cout << "                 HOSPITAL MANAGEMENT SYSTEM                 \n";
            cout << "============================================================\n";
            cout << "  1. Add New Patient Record\n";
            cout << "  2. Display All Patients\n";
            cout << "  3. Search Patient by ID\n";
            cout << "  4. Update Patient Information\n";
            cout << "  5. Delete Patient Record\n";
            cout << "  6. Calculate Hospital Bill (Generate Invoice)\n";
            cout << "  7. View Hospital Revenue & Statistics\n";
            cout << "  8. Save Records to File\n";
            cout << "  9. Exit System\n";
            cout << "============================================================\n";

            choice = getValidInt("Enter your choice (1-9): ", 1, 9);

            try {
                switch (choice) {
                    case 1:
                        hospital.addNewPatient();
                        break;
                    case 2:
                        hospital.displayAllPatients();
                        break;
                    case 3:
                        hospital.searchPatientById();
                        break;
                    case 4:
                        hospital.updatePatient();
                        break;
                    case 5:
                        hospital.deletePatient();
                        break;
                    case 6:
                        hospital.calculatePatientBill();
                        break;
                    case 7:
                        hospital.displaySummary();
                        break;
                    case 8:
                        hospital.saveRecordsToFile();
                        cout << "\n>>> All patient records have been successfully saved to file. <<<\n";
                        break;
                    case 9:
                        cout << "\nSaving records before exiting...\n";
                        hospital.saveRecordsToFile();
                        cout << "Thank you for using the Hospital Management System. Goodbye!\n";
                        break;
                    default:
                        cout << "\n[!] Invalid choice. Please select from 1 to 9.\n";
                        break;
                }
            } catch (const HospitalException& ex) {
                // Catch custom hospital application exceptions
                cout << "\n[ERROR] " << ex.what() << "\n";
            } catch (const exception& ex) {
                // Catch standard C++ exceptions
                cout << "\n[SYSTEM ERROR] " << ex.what() << "\n";
            }

            if (choice != 9) {
                cout << "\nPress Enter to return to the main menu...";
                cin.get();
            }

        } while (choice != 9);

    } catch (const exception& fatalEx) {
        cerr << "\n[FATAL ERROR] " << fatalEx.what() << "\n";
        return 1;
    }

    return 0;
}
