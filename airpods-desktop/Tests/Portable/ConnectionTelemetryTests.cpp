// Modified evaluation version, 2026-11-01. GPL-3.0-or-later.
#include "Source/Core/ConnectionTelemetry.h"
#include <cstdlib>
#include <iostream>
#include <string_view>
using Core::AirPods::ConnectionPresentation;
using Core::AirPods::ConnectionTelemetry;
namespace {
int checks = 0;
void Check(bool ok, std::string_view description)
{
    ++checks;
    if (!ok) {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(1);
    }
}
void Apply(ConnectionTelemetry &state, int event)
{
    switch (event) {
    case 0:
        state.SetConnected(true);
        break;
    case 1:
        state.SetConnected(false);
        break;
    case 2:
        state.ReceivedBattery();
        break;
    case 3:
        state.ExpireBattery();
        break;
    case 4:
        state.SetScannerAvailable(false);
        break;
    case 5:
        state.SetScannerAvailable(true);
        break;
    case 6:
        state.Reset();
        break;
    }
}
} // namespace
int main()
{
    ConnectionTelemetry s;
    Check(s.Present() == ConnectionPresentation::Unknown, "No invented OS status");
    s.ReceivedBattery();
    Check(!s.CanDisplayBattery(), "Nearby BLE packet is not an OS connection");
    s.SetConnected(true);
    Check(s.Present() == ConnectionPresentation::Waiting, "Connected before any battery packet");
    s.ReceivedBattery();
    Check(s.Present() == ConnectionPresentation::Ready && s.CanDisplayBattery(), "Live battery");
    s.SetConnected(true);
    Check(s.CanDisplayBattery(), "Duplicate OS callback retains live telemetry");
    s.ExpireBattery();
    Check(s.Present() == ConnectionPresentation::Waiting, "Broadcast timeout is not disconnect");
    s.ReceivedBattery();
    Check(s.CanDisplayBattery(), "Battery recovers after timeout");
    s.SetScannerAvailable(false);
    Check(
        s.Present() == ConnectionPresentation::ScannerUnavailable && !s.CanDisplayBattery(),
        "Scanner failure preserves OS connection, clears stale battery");
    s.ReceivedBattery();
    Check(!s.CanDisplayBattery(), "Late packet cannot revive failed scanner");
    s.SetScannerAvailable(true);
    Check(s.Present() == ConnectionPresentation::Waiting, "Scanner recovery waits for new packet");
    s.ReceivedBattery();
    s.SetConnected(false);
    Check(
        s.Present() == ConnectionPresentation::Disconnected && !s.CanDisplayBattery(),
        "OS disconnection clears batteries");
    s.ReceivedBattery();
    Check(s.Present() == ConnectionPresentation::Disconnected, "Late packet cannot reconnect");
    s.SetConnected(true);
    Check(!s.CanDisplayBattery(), "Reconnect does not reuse old battery");
    s.Reset();
    Check(
        s.Present() == ConnectionPresentation::Unknown, "Rebind removes previous identity status");

    // Exhaust every 6-event sequence; compare with an independent reference state.
    int sequences = 1;
    for (int i = 0; i < 6; ++i)
        sequences *= 7;
    for (int sequence = 0; sequence < sequences; ++sequence) {
        ConnectionTelemetry actual;
        bool known = false, connected = false, scanner = true, fresh = false;
        int events = sequence;
        for (int step = 0; step < 6; ++step) {
            int event = events % 7;
            events /= 7;
            switch (event) {
            case 0:
                if (!connected)
                    fresh = false;
                known = true;
                connected = true;
                break;
            case 1:
                known = true;
                connected = false;
                fresh = false;
                break;
            case 2:
                fresh = connected && scanner;
                break;
            case 3:
                fresh = false;
                break;
            case 4:
                scanner = false;
                fresh = false;
                break;
            case 5:
                scanner = true;
                break;
            case 6:
                known = false;
                connected = false;
                scanner = true;
                fresh = false;
                break;
            }
            Apply(actual, event);
            auto expected = !known       ? ConnectionPresentation::Unknown
                            : !connected ? ConnectionPresentation::Disconnected
                            : !scanner   ? ConnectionPresentation::ScannerUnavailable
                            : fresh      ? ConnectionPresentation::Ready
                                         : ConnectionPresentation::Waiting;
            Check(actual.Present() == expected, "Event sequence connection status");
            Check(
                actual.CanDisplayBattery() == (known && connected && scanner && fresh),
                "Event sequence telemetry validity");
        }
    }
    std::cout << "PASS: 14 regression checks + " << sequences << " event sequences (" << checks
              << " assertions)\n";
}
