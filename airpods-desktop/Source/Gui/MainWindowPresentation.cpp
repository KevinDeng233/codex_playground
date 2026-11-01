//
// AirPodsDesktop - AirPods Desktop User Experience Enhancement Program.
// Copyright (C) 2021-2022 SpriteOvO
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//

#include "MainWindowPresentation.h"

#include <algorithm>

namespace Gui {
namespace {

BatteryPresentation PresentBattery(const Core::AirPods::Details::BasicState &state)
{
    if (!state.battery.Available()) {
        return {};
    }

    return {
        .visible = true,
        .charging = state.isCharging,
        .value = state.battery.Value(),
    };
}

} // namespace

void MainWindowViewModel::SetDeviceIdentity(const QString &name)
{
    _deviceName = name;
    if (!name.isEmpty()) {
        _status = Status::Disconnected;
    }
    _state.reset();
    _connection.Reset();
}

void MainWindowViewModel::SetDeviceConnected(bool connected)
{
    _connection.SetConnected(connected);
    if (!connected) {
        _state.reset();
    }
}

void MainWindowViewModel::BatteryUnavailable()
{
    _connection.ExpireBattery();
    _state.reset();
}

void MainWindowViewModel::UpdateState(const Core::AirPods::State &state)
{
    _connection.ReceivedBattery();
    if (!_connection.CanDisplayBattery()) {
        return;
    }
    _status = Status::Updating;
    _state = state;
    if (_deviceName.isEmpty()) {
        _deviceName = state.displayName;
    }
}

void MainWindowViewModel::Available()
{
    _connection.SetScannerAvailable(true);
    if (_status == Status::Unavailable) {
        _status = Status::Disconnected;
    }
}

void MainWindowViewModel::Unavailable()
{
    _connection.SetScannerAvailable(false);
    _status = Status::Unavailable;
    _state.reset();
}

void MainWindowViewModel::Disconnect()
{
    if (_status == Status::Unbind) {
        return;
    }

    _status = Status::Disconnected;
    _connection.SetConnected(false);
    _state.reset();
}

void MainWindowViewModel::Bind()
{
    _deviceName.clear();
    _status = Status::Disconnected;
    _connection.Reset();
    _state.reset();
}

void MainWindowViewModel::Unbind()
{
    _deviceName.clear();
    _connection.Reset();
    _status = Status::Unbind;
    _state.reset();
}

MainWindowPresentation MainWindowViewModel::Present() const
{
    MainWindowPresentation result;

    switch (_status) {
    case Status::Unavailable:
    case Status::Disconnected:
    case Status::Unbind:
        result.title = DisplayableStatus(_status);
        break;
    default:
        break;
    }

    if (_status == Status::Unbind) {
        result.buttonAction = ButtonAction::Bind;
    }

    using Core::AirPods::ConnectionPresentation;
    switch (_connection.Present()) {
    case ConnectionPresentation::Unknown:
        result.connectionStatus = QObject::tr("Waiting for Windows device status");
        break;
    case ConnectionPresentation::Disconnected:
        result.connectionStatus = QObject::tr("Disconnected");
        break;
    case ConnectionPresentation::Waiting:
        result.connectionStatus = QObject::tr("Connected · Waiting for battery");
        break;
    case ConnectionPresentation::Ready:
        result.connectionStatus = QObject::tr("Connected · Battery received");
        break;
    case ConnectionPresentation::ScannerUnavailable:
        result.connectionStatus = QObject::tr("Connected · Battery scanner unavailable");
        break;
    }
    if (!_deviceName.isEmpty()) {
        result.title = _deviceName;
    }
    if (!_state.has_value() || !_connection.CanDisplayBattery()) {
        return result;
    }

    result.title = _state->displayName;
    result.animationModel = _state->model;
    result.leftBattery = PresentBattery(_state->pods.left);
    result.rightBattery = PresentBattery(_state->pods.right);
    result.caseBattery = PresentBattery(_state->caseBox);
    return result;
}

AnimationPresentation GetAnimationPresentation(Core::AirPods::Model model)
{
    using Core::AirPods::Model;

    switch (model) {
    case Model::AirPods_1:
        return {"qrc:/Resource/Video/AirPods_1.avi", {800, 400}};
    case Model::AirPods_2:
        return {"qrc:/Resource/Video/AirPods_2.avi", {800, 400}};
    case Model::AirPods_3:
        return {"qrc:/Resource/Video/AirPods_3.avi", {900, 450}};
    case Model::AirPods_4:
        return {"qrc:/Resource/Video/AirPods_4.avi", {900, 450}};
    case Model::AirPods_4_ANC:
        return {"qrc:/Resource/Video/AirPods_4_ANC.avi", {900, 450}};
    case Model::AirPods_5:
        return {"qrc:/Resource/Video/AirPods_4_ANC.avi", {900, 450}};
    case Model::AirPods_Pro:
        return {"qrc:/Resource/Video/AirPods_Pro.avi", {900, 450}};
    case Model::AirPods_Pro_2:
    case Model::AirPods_Pro_2_USB_C:
        return {"qrc:/Resource/Video/AirPods_Pro_2.avi", {900, 450}};
    case Model::AirPods_Pro_3:
        return {"qrc:/Resource/Video/AirPods_Pro_3.avi", {900, 450}};
    case Model::AirPods_Max:
    case Model::AirPods_Max_USB_C:
        return {"qrc:/Resource/Video/AirPods_Max.avi", {600, 650}, true};
    case Model::Beats_Fit_Pro:
        return {"qrc:/Resource/Video/Beats_Fit_Pro.avi", {900, 450}};
    case Model::Powerbeats_3:
    case Model::Beats_X:
    case Model::Beats_Solo3:
    default:
        return {"qrc:/Resource/Video/AirPods_1.avi", {800, 400}};
    }
}

QString AnimationPresentation::FallbackResource() const
{
    if (!resource.startsWith("qrc:/Resource/Video/") || !resource.endsWith(".avi")) {
        return {};
    }
    auto result = resource;
    result.replace("qrc:/Resource/Video/", ":/Resource/Image/Animation/");
    result.chop(4);
    return result + ".png";
}

QPoint PopupPosition(const QRect &availableGeometry, QSize windowSize, QSize margin)
{
    return {
        (std::max)(
            availableGeometry.left(),
            availableGeometry.right() + 1 - windowSize.width() - margin.width()),
        (std::max)(
            availableGeometry.top(),
            availableGeometry.bottom() + 1 - windowSize.height() - margin.height())};
}

} // namespace Gui
