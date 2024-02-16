#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <cassert>
#include <algorithm>
#include "split.h"
using namespace std;

struct Customer {
    int customerID;
    string name;
    string address;
    string city;
    string state;
    string zipCode;
    string phoneNumber;
    string emailAddress;

    string print_detail() const {
        ostringstream outSS;

        outSS << "Customer ID #" << customerID << endl;
        outSS << name << ", ph. " << phoneNumber << ", email: " << emailAddress << endl;
        outSS << address << endl;
        outSS << city << ", " << state << " " << zipCode << endl;
        return outSS.str();
    }
};

vector <Customer> customerList;

int findCustomerIndex(const int toSearchFor) {

    for (int i = 0; i < customerList.size(); ++i) {
        if (customerList[i].customerID == toSearchFor) {
            return i;
        }
    }

    return -1; //If Customer ID does not exist
}

void read_customers(const string& file) {
    ifstream inFS;
    string customerString;
    string filePointer;

    inFS.open(file);

    while (!inFS.fail()) {//Loop until end of file is reached
        Customer cust;
        while (customerString.find("@") == string::npos) { //Loop until customer email is read from file
            inFS >> filePointer;
            customerString += filePointer;

            //If @ symbol is still not in the string, a space is added
            if (customerString.find("@") == string::npos) {
                customerString += " ";
            }
        }

        auto fields = split(customerString, ','); //Split customerString by commas

        //Set member data
        cust.customerID   = stoi(fields[0]);
        cust.name         = fields[1];
        cust.address      = fields[2];
        cust.city         = fields[3];
        cust.state        = fields[4];
        cust.zipCode      = fields[5];
        cust.phoneNumber  = fields[6];
        cust.emailAddress = fields[7];

        //If the phone number has a space in it, replace it with an empty string
        if (cust.phoneNumber.find(" ") != string::npos) {
            cust.phoneNumber.replace(8, 1, "");
        }

        customerList.push_back(cust);

        customerString = ""; //Reset string before looping to next customer in file
        inFS >> filePointer;
        customerString += filePointer + " ";
    }
}

struct Item {
    int itemID;
    string description;
    double price;
};

vector <Item> itemList;

void read_items(const string& file) {
    ifstream inFS;
    string itemString;

    inFS.open(file);

    getline(inFS, itemString); //Get full line of file
    while (!inFS.fail()) { //Loop until end of file
        Item newItem;
        
        auto fields = split(itemString, ','); //Get comma seperated vector

        //Set member data
        newItem.itemID      = stoi(fields[0]);
        newItem.description =      fields[1];
        newItem.price       = stod(fields[2]);

        itemList.push_back(newItem);

        getline(inFS, itemString); //Get next line
    }
}

int findItemIndex(const int toSearchFor) {
    for (int i = 0; i < itemList.size(); ++i) {
        if (itemList[i].itemID == toSearchFor) {
            return i;
        }
    }

    return -1; //If Item ID does not exist
}

class LineItem {
    int item_id;
    int qty;

    public:
        LineItem(int item_id, int qty) {
            this->item_id = item_id;
            this->qty     = qty;
        }

        friend bool operator<(const LineItem& item1, const LineItem& item2) {
        return item1.item_id < item2.item_id;
        }

        double sub_total() {
            return itemList[findItemIndex(item_id)].price * qty;
        }

        int Get_itemID() {
            return item_id;
        }

        int Get_qty() {
            return qty;
        }

        string print_detail() {
            ostringstream outSS;
            Item currItem = itemList[findItemIndex(Get_itemID())];

            outSS << fixed << setprecision(2);
            outSS << "Item " << currItem.itemID << ": \"" << currItem.description << "\", ";
            outSS << Get_qty() << " @ " << currItem.price << endl;
            return outSS.str();
        }      
};


class Payment {
    private: double amount;

    public:
        virtual string print_detail() const {
            ostringstream outSS;
            outSS << fixed << setprecision(2);
            outSS << "Amount: $" << amount << ", Paid by ";
            return outSS.str();
        }

        void set_amount(double amount) {
            this->amount = amount;
        }
};

class Credit: public Payment {
    string card_number;
    string expiration;

    public:
        Credit(string card_number, string expiration) {
            this->card_number = card_number;
            this->expiration  = expiration;
    }

        string print_detail() const override {
            ostringstream outSS;

            outSS << Payment::print_detail();
            outSS << "Credit card " << card_number << ", exp. " << expiration << endl; 

            return outSS.str();
        }
};

class PayPal: public Payment {
    string paypal_id;

    public:
        PayPal(string paypal_id) {
            this->paypal_id = paypal_id;
        }

        string print_detail() const override {
            ostringstream outSS;

            outSS << Payment::print_detail();
            outSS << "Paypal ID: " << paypal_id << endl;

            return outSS.str();
        }
};

class WireTransfer: public Payment {
    string bank_id;
    string account_id;

    public:
        WireTransfer(string bank_id, string account_id) {
            this->bank_id    = bank_id;
            this->account_id = account_id;
        }
        
        string print_detail() const override {
            ostringstream outSS;

            outSS << Payment::print_detail();
            outSS << "Wire transfer from Bank ID ";
            outSS << bank_id << ", Account# " << account_id << endl;

            return outSS.str();
        }
};

class Order {
    int order_id;
    string order_date;
    int cust_id;
    vector <LineItem> line_items;
    Payment* payment; 

    public:
        Order(int order_id, string order_date, int cust_id, vector <LineItem> line_items, Payment* payment) {
            this->order_id    = order_id;
            this->order_date  = order_date;
            this->cust_id     = cust_id;
            this->line_items  = line_items;
            this->payment     = payment;
            this->payment->set_amount(total());
        }
        
        //Copy constructor 
        Order(const Order& o) {
            order_id   = o.order_id;
            order_date = o.order_date;
            cust_id    = o.cust_id;

            //Copy line_items
            line_items.clear(); // Clear existing items
            for (const LineItem& item: o.line_items) {
                line_items.push_back(item);
            }

            //Copy payment
            if (dynamic_cast<Credit*>(o.payment)) {
                payment = new Credit(*dynamic_cast<Credit*>(o.payment));
            }

            else if (dynamic_cast<PayPal*>(o.payment)) {
                payment = new PayPal(*dynamic_cast<PayPal*>(o.payment));
            }

            else if (dynamic_cast<WireTransfer*>(o.payment)) {
                payment = new WireTransfer(*dynamic_cast<WireTransfer*>(o.payment));
            }
        }

        ~Order() {
            delete payment;
        }
        
        double total() {
            double total = 0;
            for (LineItem element: line_items) {
                total += element.sub_total();
            }

            return total;
        }

        string print_order() const {
            ostringstream outSS;
            outSS << "===========================" << endl;
            outSS << "Order #" << order_id << ", Date: " << order_date << endl;
            outSS << payment->print_detail() << endl;
            outSS << customerList[findCustomerIndex(cust_id)].print_detail() << endl;
            outSS << "Order Detail:" << endl;
            for (LineItem element: line_items) {
                outSS << "      " << element.print_detail();
            }

            return outSS.str();
        }
};

list <Order> orders;

vector <LineItem> GetLineItems(const vector <string> &fields) {
    int item_id, item_qty;
    vector <LineItem> lineItems;

    for (int i = 3; i < fields.size(); ++i) {
            auto itemID_qty = split(fields[i], '-');
            item_id  = stoi(itemID_qty[0]);
            item_qty = stoi(itemID_qty[1]);
            lineItems.push_back(LineItem(item_id, item_qty));
        }
    
    sort(lineItems.begin(), lineItems.end());
    return lineItems;
}

void read_orders(string file) {
    ifstream inFS;
    string orderString, paymentString, order_date;
    int cust_id, order_id, item_id, item_qty;
    Payment* payment;

    inFS.open(file);
    getline(inFS, orderString);     
    getline(inFS, paymentString);   //Get both lines of order

    while(!inFS.fail()) { //Loop until end of file
        //Get vector for both lines
        auto orderFields   = split(orderString, ',');
        auto paymentFields = split(paymentString, ',');

        //Prepare orderString parameters
        cust_id    = stoi(orderFields[0]);
        order_id   = stoi(orderFields[1]);
        order_date = (orderFields[2]);
        vector <LineItem> lineItems = GetLineItems(orderFields);

        //Prepare payment parameter
        if (paymentFields[0] == "1") {
            payment = new Credit(paymentFields[1], paymentFields[2]);
        }

        if (paymentFields[0] == "2") {
            payment = new PayPal(paymentFields[1]);
        }
        
        if (paymentFields[0] == "3") {
            payment = new WireTransfer(paymentFields[1], paymentFields[2]);
        }

        Order newOrder = Order(order_id, order_date, cust_id, lineItems, payment);
        orders.push_back(newOrder);

        getline(inFS, orderString);
        getline(inFS, paymentString); //Get next 2 lines
    }

    inFS.close();
}

void one_customer_order() { //This is from Project 5: OOP. Part 1, does not seem to be used in part 2
    int customerSearch, itemSearch;
    int numberOfItems   = 0;
    double runningTotal = 0;
    Customer targetCustomer;
    Item targetItem;
    
    cout << fixed << setprecision(2); //Set precision for whole function
    
    cout << "Enter a customer ID number to search for:" << endl;
    cin >> customerSearch;

    if (findCustomerIndex(customerSearch) == -1) {
        throw runtime_error(to_string(customerSearch));
    }

    //customerSearch is a valid ID
    targetCustomer = customerList[findCustomerIndex(customerSearch)];
    cout << endl;
    cout << "Customer found - " << targetCustomer.name << endl << endl;

    cout << "Enter an item ID number to purchase, or 0 to exit:" << endl;
    cin >> itemSearch;

    while (itemSearch != 0) {
        if (findItemIndex(itemSearch) == -1) { //itemSearch is not a valid ID
            cout << "Item ID " << itemSearch << " does not exist. Please try again." << endl;
        }

        else { //itemSearch is a valid ID
            targetItem = itemList[findItemIndex(itemSearch)];
            cout << targetItem.description << " - $" << targetItem.price << endl << endl;
            runningTotal += targetItem.price;
            ++numberOfItems;
        }
        cout << "Enter an item ID number to purchase, or 0 to exit:" << endl;
        cin >> itemSearch; //Next item search
    }

    cout << endl;
    cout << "Total number of items purchased: " << numberOfItems << endl;
    cout << "Total cost:                      " << runningTotal  << endl;
}

int main() {
    read_customers("customers.txt");
    read_items("items.txt");
    read_orders("orders.txt");
    
    ofstream ofs("order_report.txt");
    for (const auto& order: orders)
        ofs << order.print_order() << endl;      
}