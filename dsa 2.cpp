Algorithm of the parking system
START
  READ Total_Slots from Database
  READ Occupied_Slots where status = 'Occupied'
  COMPUTE Available_Slots = Total_Slots - Occupied_Slots
  IF Available_Slots > 0 THEN
      DISPLAY Available_Slots and "WELCOME" on LED Board
  ELSE
      DISPLAY "PARKING FULL" on LED Board
  ENDIF
END
START
  CAPTURE Vehicle_Plate via ANPR Camera or Sensor
  QUERY Database for Available_Slots
  IF Available_Slots IS Empty THEN
      DISPLAY "Sorry, Parking Full"
      EXIT
  ELSE
      ALLOCATE first available Slot_Number
      CREATE Record: Ticket_ID, Vehicle_Plate, Entry_Time (Current Time), Slot_Number, Status ('Active')
      UPDATE Slot_Number status to 'Occupied' in Database
      PRINT Ticket (or send Digital Ticket via SMS)
      TRIGGER Entry Barrier Open
      WAIT for vehicle to pass sensor
      TRIGGER Entry Barrier Close
  ENDIF
END
START
  CAPTURE Vehicle_Plate at Exit Gate
  QUERY Database for Active Record using Vehicle_Plate
  IF Record Found THEN
      SET Exit_Time = Current Time
      COMPUTE Duration_Minutes = (Exit_Time - Entry_Time) in minutes
      COMPUTE Duration_Hours = CEIL(Duration_Minutes / 60)
      
      // Kenyan Parking Rate Example (e.g., KES 100 for the first hour, KES 50 per extra hour)
      IF Duration_Hours <= 1 THEN
          SET Amount_Due = 100
      ELSE
          SET Amount_Due = 100 + ((Duration_Hours - 1) * 50)
      ENDIF
      
      DISPLAY Amount_Due to Driver
      PROMPT Payment (M-Pesa / Card / Cash)
      
      IF Payment_Successful THEN
          UPDATE Record: Exit_Time, Amount_Paid, Status ('Completed')
          UPDATE Slot_Number status to 'Available'
          TRIGGER Exit Barrier Open
          WAIT for vehicle to pass sensor
          TRIGGER Exit Barrier Close
      ELSE
          DISPLAY "Payment Failed. Please try again."
      ENDIF
  ELSE
      DISPLAY "Error: Vehicle Record Not Found"
  ENDIF
END


question 2;
      Queue (FIFO - First In, First Out)
•	Use: Managing the sequence of cars waiting at the entry and exit barriers.
•	Reason: Ensures fair processing of vehicles in the exact order they arrived at the sensors, preventing race conditions during peak hours.


question 3;

DATABASE


CREATE TABLE Slots (
    slot_id VARCHAR(10) PRIMARY KEY
    floor_number INT NOT NULL,
    slot_type VARCHAR(20) DEFAULT 'Standard'
    status VARCHAR(20) CHECK (status IN ('Available', 'Occupied', 'Maintenance')) DEFAULT 'Available'
);

CREATE TABLE Parking_Transactions (
    transaction_id SERIAL PRIMARY KEY,
    vehicle_plate VARCHAR(15) NOT NULL,
    slot_id VARCHAR(10) REFERENCES Slots(slot_id),
    entry_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    exit_time TIMESTAMP NULL,
    status VARCHAR(20) CHECK (status IN ('Active', 'Completed')) DEFAULT 'Active'
);

CREATE TABLE Payments (
    payment_id SERIAL PRIMARY KEY,
    transaction_id INT REFERENCES Parking_Transactions(transaction_id),
    amount_paid DECIMAL(10, 2) NOT NULL,
    payment_method VARCHAR(30) CHECK (payment_method IN ('M-Pesa)),
    payment_reference VARCHAR(50) UNIQUE, 
    payment_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

program;;

#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std;

struct Transaction {
    int transactionId;
    string vehiclePlate;
    string slotId;
    chrono::system_clock::time_point entryTime;
    chrono::system_clock::time_point exitTime;
    bool isActive;
    double amountPaid;
};

struct Slot {
    string slotId;
    int floorNumber;
    string status;
};

class ParkingSystem {
private:
    int totalSlots;
    int occupiedSlotsCount;
    int nextTransactionId;
    unordered_map<string, Transaction> activeTransactions; 
    unordered_map<string, Slot> slotInventory;             
    queue<string> entryLaneQueue;                          
    queue<string> exitLaneQueue;                           

public:
    ParkingSystem(int columns, int rows) {
        this->occupiedSlotsCount = 0;
        this->nextTransactionId = 1001;
        this->totalSlots = columns * rows;

        for (int i = 1; i <= columns; ++i) {
            for (int j = 1; j <= rows; ++j) {
                string id = "FL1-" + to_string(i) + (char)('A' + j - 1);
                slotInventory[id] = {id, 1, "Available"};
            }
        }
    }

    void displayAvailableSlots() {
        int available = totalSlots - occupiedSlotsCount;
        cout << "\n============================================\n";
        cout << " [EXTERIOR LED DISPLAY BOARD]\n";
        if (available > 0) {
            cout << " AVAILABLE SLOTS: " << available << " | [ WELCOME ]\n";
        } else {
            cout << " STATUS: [ PARKING FULL ]\n";
        }
        cout << "============================================\n";
    }

    string getNextAvailableSlot() {
        for (auto const& [id, slot] : slotInventory) {
            if (slot.status == "Available") {
                return id;
            }
        }
        return "";
    }

    void enqueueVehicleArrival(string plateNumber) {
        entryLaneQueue.push(plateNumber);
        cout << "-> Vehicle " << plateNumber << " detected on approach loop sensor. Enqueued.\n";
    }

    void processVehicleEntry() {
        if (entryLaneQueue.empty()) {
            cout << "[Gate Controller] Entry lane clear. No cars waiting.\n";
            return;
        }

        string plate = entryLaneQueue.front(); 
        int available = totalSlots - occupiedSlotsCount;

        if (available <= 0) {
            cout << "[Gate Controller] Access Denied to " << plate << ". Parking lot is full!\n";
            entryLaneQueue.pop();
            return;
        }

        string allocatedSlot = getNextAvailableSlot();
        
        Transaction newTx = {
            nextTransactionId++,
            plate,
            allocatedSlot,
            chrono::system_clock::now(),
            chrono::system_clock::now(), 
            true,
            0.0
        };

        activeTransactions[plate] = newTx;
        slotInventory[allocatedSlot].status = "Occupied";
        occupiedSlotsCount++;

        cout << "\n[ENTRY BARRIER OPENED] for Plate: " << plate << "\n";
        cout << " Allocated Slot: " << allocatedSlot << " | Ticket ID: " << newTx.transactionId << "\n";
        
        entryLaneQueue.pop(); 
        cout << " [Barrier Loop Sensor] Vehicle cleared. Entry gate closed.\n";
    }

    void enqueueVehicleExit(string plateNumber) {
        exitLaneQueue.push(plateNumber);
        cout << "-> Vehicle " << plateNumber << " arrived at exit barrier approach loop. Enqueued.\n";
    }

    double calculateFee(long long durationMinutes) {
        if (durationMinutes <= 30) return 0.0;
        if (durationMinutes <= 120) return 50.0;  
        if (durationMinutes <= 240) return 100.0; 
        if (durationMinutes <= 360) return 300.0; 
        return 500.0;                             
    }

    void processVehicleExit(long long simulatedDurationMinutes) {
        if (exitLaneQueue.empty()) {
            cout << "[Gate Controller] Exit lane clear.\n";
            return;
        }

        string plate = exitLaneQueue.front(); 
        
        if (activeTransactions.find(plate) == activeTransactions.end() || !activeTransactions[plate].isActive) {
            cout << "[Error] No active record found for vehicle plate: " << plate << "\n";
            exitLaneQueue.pop();
            return;
        }

        Transaction& tx = activeTransactions[plate];
        double amountDue = calculateFee(simulatedDurationMinutes);

        cout << "\n============================================\n";
        cout << " [EXIT TERMINAL PROMPT] Plate: " << plate << "\n";
        cout << " Total Time Logged: " << simulatedDurationMinutes << " Minutes\n";
        cout << " TOTAL AMOUNT DUE  : KSHS. " << fixed << setprecision(2) << amountDue << "\n";
        cout << "============================================\n";

        if (amountDue > 0.0) {
            cout << " Sending M-Pesa STK Push Prompt to Driver's Phone...\n";
            cout << " [Simulation Mode] Processing payment clearance... [SUCCESS]\n";
            tx.amountPaid = amountDue;
        } else {
            cout << " Duration within Free Grace Tier. No processing fee required.\n";
        }

        tx.exitTime = tx.entryTime + chrono::minutes(simulatedDurationMinutes);
        tx.isActive = false;
        
        slotInventory[tx.slotId].status = "Available";
        occupiedSlotsCount--;

        cout << "[EXIT BARRIER OPENED] Payment verified. Goodbye!\n";
        
        exitLaneQueue.pop(); 
        cout << " [Barrier Loop Sensor] Vehicle cleared. Exit gate locked.\n";
    }
};

int main() {
    ParkingSystem mallParking(2, 2);

    cout << "--- ENTRANCE GATE WORKFLOW SIMULATION ---\n";
    mallParking.displayAvailableSlots();

    mallParking.enqueueVehicleArrival("KAA 123Z");
    mallParking.enqueueVehicleArrival("KBC 456Y");
    mallParking.enqueueVehicleArrival("KCD 789X");

    mallParking.processVehicleEntry();
    mallParking.processVehicleEntry();
    mallParking.processVehicleEntry();

    mallParking.displayAvailableSlots();

    cout << "\n--- EXIT GATE & PAYMENT STRUCTURE SIMULATION ---\n";
    
    mallParking.enqueueVehicleExit("KAA 123Z");
    mallParking.processVehicleExit(20); 

    mallParking.enqueueVehicleExit("KBC 456Y");
    mallParking.processVehicleExit(210);

    mallParking.enqueueVehicleExit("KCD 789X");
    mallParking.processVehicleExit(420);

    mallParking.displayAvailableSlots();

    return 0;
}
