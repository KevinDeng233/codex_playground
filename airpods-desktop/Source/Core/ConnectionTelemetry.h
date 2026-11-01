// Modified evaluation version, 2026-11-01. GPL-3.0-or-later.
#pragma once

namespace Core::AirPods {

// Only the OS connection callback may establish connectivity. BLE advertisements
// carry battery telemetry; their absence does not prove a Bluetooth disconnect.
enum class ConnectionPresentation { Unknown, Disconnected, Waiting, Ready, ScannerUnavailable };

class ConnectionTelemetry
{
public:
    void SetConnected(bool connected)
    {
        _known = true;
        if (_connected != connected || !connected) {
            _fresh = false;
        }
        _connected = connected;
    }
    void SetScannerAvailable(bool available)
    {
        _scannerAvailable = available;
        if (!available) {
            _fresh = false;
        }
    }
    void ReceivedBattery()
    {
        _fresh = _connected && _scannerAvailable;
    }
    void ExpireBattery()
    {
        _fresh = false;
    }
    void Reset()
    {
        *this = ConnectionTelemetry{};
    }
    bool CanDisplayBattery() const
    {
        return _connected && _scannerAvailable && _fresh;
    }
    ConnectionPresentation Present() const
    {
        if (!_known)
            return ConnectionPresentation::Unknown;
        if (!_connected)
            return ConnectionPresentation::Disconnected;
        if (!_scannerAvailable)
            return ConnectionPresentation::ScannerUnavailable;
        return _fresh ? ConnectionPresentation::Ready : ConnectionPresentation::Waiting;
    }

private:
    bool _known{false};
    bool _connected{false};
    bool _scannerAvailable{true};
    bool _fresh{false};
};

} // namespace Core::AirPods
