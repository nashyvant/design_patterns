
#include<iostream>
using namespace std;
#include <stdint.h>

/* 
 * Creating a singleton class for the test equipment instance since one
 * HW setup will only need to access to that single instance of the test equipment. 
 * That same test equipment instance can be used at any point in the program
 */
enum class eWaveform
{
    None,
    ASK,
    FSK,
    QPSK,
    QAM_64,
    QAM_256
};


enum class ePort
{
    None,
    Port_0,
    Port_1,
    Port_2,
    Port_3
};

 class TestEquipment
 {
    public:
        bool radio_power(uint8_t power_on);
        bool generate_signals(const eWaveform&);
        bool analyze_signals(const ePort&);
        eWaveform get_tx_wvfm();
        ePort get_rx_port();
        static TestEquipment* get_instance();

        constexpr const char* wvfm_to_str(const eWaveform&) const;
        constexpr const char* port_to_str(const ePort&) const;

    private:
        TestEquipment();
        static TestEquipment* __instance;
        bool is_on;
        bool is_transmit;
        bool is_receive;

        eWaveform tx_wvfm;
        ePort rx_port; 
 };

// Initialize static member
TestEquipment* TestEquipment::__instance = nullptr;

TestEquipment::TestEquipment() :
    tx_wvfm(eWaveform::None), rx_port(ePort::None), is_on(false), is_transmit(false), is_receive(false)
{ }

TestEquipment*
TestEquipment::get_instance()
{
    if(__instance == nullptr)
        __instance = new TestEquipment();
    
    return __instance;
}

bool
TestEquipment::radio_power(uint8_t power_on)
{
    if(!power_on) //turn off
    {
        if(is_transmit || is_receive) //cannot turn off is TX or RX is running
        {
            cout<<"Cannot turn off radio when TX or RX is on\n";
            return false;
        }
        is_on = false;
    }  
    else
        is_on = true;

    return true;
}

bool
TestEquipment::generate_signals(const eWaveform& wvfm) {
    if(is_on)
    {
        if(!is_transmit) //not currently transmitting
        {
            is_transmit = true;
            tx_wvfm = wvfm;
        }
        else
        {
            if(wvfm == eWaveform::None)
            {
                is_transmit = false;
                tx_wvfm = wvfm;
            }
            else
            {
                cout<<"Already transmitting! First turn off, change wvfm and then turn on trasnmitter."<<endl;
                return false;
            }
        }

        cout<<"TX is transmitting "<<wvfm_to_str(tx_wvfm)<<endl;
        return true;
    }

    cout<<"Turn on the equipment first!"<<endl;
    return false;
}

bool 
TestEquipment::analyze_signals(const ePort& port){
    if(is_on)
    {
        if(!is_receive) //currently not receiving
        {
            is_receive = true;
            rx_port = port;
        }
        else
        {
            if(port == ePort::None)
            {
                is_receive = false;
                rx_port = port;
            }
            else
            {
                cout<<"Already receiving! First turn off, change port and then receive."<<endl;
                return false;
            }
        }

        cout<<"RX is receiving at "<<port_to_str(rx_port)<<endl;
        return true;
    }

    cout<<"Turn on the equipment first!"<<endl;
    return false;
} 

eWaveform 
TestEquipment::get_tx_wvfm() {
    return tx_wvfm;

}

ePort
TestEquipment::get_rx_port() {
    return rx_port;
}

constexpr const char* 
TestEquipment::wvfm_to_str(const eWaveform& w) const
{
    switch(w)
    {
        case eWaveform::ASK: return "ASK";
        case eWaveform::FSK: return "FSK";
        case eWaveform::QPSK: return "QPSK";
        case eWaveform::QAM_64: return "QAM_64";
        case eWaveform::QAM_256: return "QAM_256";
        default: return "None";
    }
}

constexpr const char* 
TestEquipment::port_to_str(const ePort& p) const
{
    switch(p)
    {
        case ePort::Port_0: return "Port_0";
        case ePort::Port_1: return "Port_1";
        case ePort::Port_2: return "Port_2";
        case ePort::Port_3: return "Port_3";
        default: return "None";
    }
}


int main() {
    TestEquipment* te = TestEquipment::get_instance();
    te->radio_power(1);
    te->generate_signals(eWaveform::QPSK);
    te->analyze_signals(ePort::Port_1);

    cout << "Current TX Waveform: " << te->wvfm_to_str(te->get_tx_wvfm()) << endl;
    cout << "Current RX Port: " <<te->port_to_str(te->get_rx_port()) << endl;

    te->radio_power(0); //Error! turning off before turning off TX/RX
    te->analyze_signals(ePort::Port_3); //Error! changing port while receive in progress
    te->analyze_signals(ePort::None);
    te->analyze_signals(ePort::Port_3);
    te->analyze_signals(ePort::None);
    te->generate_signals(eWaveform::QAM_256);//Error! changing wvfm while TX in progress
    te->generate_signals(eWaveform::None);
    te->generate_signals(eWaveform::QAM_256);
    te->generate_signals(eWaveform::None);
    te->radio_power(0);

    return 0;
}