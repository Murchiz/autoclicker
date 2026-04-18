import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    implicitHeight: randomFrequencyEnabled ? 160 : 130
    height: implicitHeight
    color: "#2d2d2d"
    radius: 10
    border.color: isActive ? "#4CAF50" : "#444444"
    border.width: isActive ? 3 : 1

    property int profileId
    property string targetButton
    property int targetButtonCode
    property string keybind
    property int keybindCode
    property double frequency
    property bool randomFrequencyEnabled
    property double maxFrequency
    property int mode
    property bool isActive

    function normalizedFrequencyText(value) {
        return Number(value).toFixed(2);
    }

    function commitFrequency() {
        const parsed = Number.parseFloat(frequencyField.text.replace(",", "."));
        if (Number.isNaN(parsed) || parsed <= 0) {
            frequencyField.text = normalizedFrequencyText(root.frequency);
            return;
        }
        const clamped = Math.max(0.01, Math.min(100.0, parsed));
        frequencyField.text = normalizedFrequencyText(clamped);
        autoClickerController.setFrequency(profileId, clamped);
    }

    function commitMaxFrequency() {
        const parsed = Number.parseFloat(maxFrequencyField.text.replace(",", "."));
        if (Number.isNaN(parsed) || parsed <= 0) {
            maxFrequencyField.text = normalizedFrequencyText(root.maxFrequency);
            return;
        }
        const clamped = Math.max(root.frequency, Math.min(100.0, parsed));
        maxFrequencyField.text = normalizedFrequencyText(clamped);
        autoClickerController.setMaxFrequency(profileId, clamped);
    }

    MouseArea {
        id: rootMA
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        radius: 10
        border.color: "#5a9fd4"
        border.width: rootMA.containsMouse ? 2 : 0
        Behavior on border.width {
            NumberAnimation {
                duration: 150
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // Status Bar
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 6
            color: isActive ? "#4CAF50" : "#555555"
            radius: 3
        }

        // Settings Columns
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            // Column 1: Target & Hotkey
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: 140
                spacing: 2

                Text {
                    text: translationManager.currentLanguage && translationManager.textFor("target_button")
                    font.pixelSize: 11
                    color: "#888888"
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: targetArea.containsMouse || (autoClickerController.isListeningForTarget && autoClickerController.listeningProfileId === profileId) ? "#3d5a80" : "#3a3a3a"
                    radius: 6
                    border.color: targetButton !== "" ? "#5a9fd4" : "#555555"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: targetButton !== "" ? targetButton : (translationManager.currentLanguage && translationManager.textFor("set_target"))
                        font.pixelSize: 13
                        color: targetButton !== "" ? "#ffffff" : "#777777"
                        font.bold: targetButton !== ""
                    }
                    MouseArea {
                        id: targetArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: autoClickerController.startListeningForTarget(profileId)
                    }
                }

                Item {
                    Layout.preferredHeight: 4
                }

                Text {
                    text: translationManager.currentLanguage && translationManager.textFor("keybind")
                    font.pixelSize: 11
                    color: "#888888"
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: keybindArea.containsMouse || (autoClickerController.isListeningForKeybind && autoClickerController.listeningProfileId === profileId) ? "#3d5a80" : "#3a3a3a"
                    radius: 6
                    border.color: keybind !== "" ? "#5a9fd4" : "#555555"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: keybind !== "" ? keybind : (translationManager.currentLanguage && translationManager.textFor("set_keybind"))
                        font.pixelSize: 13
                        color: keybind !== "" ? "#ffffff" : "#777777"
                        font.bold: keybind !== ""
                    }
                    MouseArea {
                        id: keybindArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: autoClickerController.startListeningForKeybind(profileId)
                    }
                }
            }

            // Column 2: Intervals
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: 150
                spacing: 2

                Text {
                    text: randomFrequencyEnabled ? (translationManager.currentLanguage && translationManager.textFor("min_frequency_sec")) : (translationManager.currentLanguage && translationManager.textFor("frequency_sec"))
                    font.pixelSize: 11
                    color: "#888888"
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: "#3a3a3a"
                    radius: 6
                    border.color: frequencyField.activeFocus ? "#5a9fd4" : "#555555"
                    border.width: 1
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 2
                        spacing: 2
                        TextField {
                            id: frequencyField
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: root.normalizedFrequencyText(root.frequency)
                            validator: RegularExpressionValidator {
                                regularExpression: /^\d{0,3}[.,]?\d{0,2}$/
                            }
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            selectByMouse: true
                            background: Item {}
                            onEditingFinished: {
                                root.commitFrequency();
                                frequencyField.focus = false;
                            }
                        }
                        ColumnLayout {
                            Layout.preferredWidth: 24
                            Layout.fillHeight: true
                            spacing: 1
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: incArea.pressed ? "#4CAF50" : (incArea.containsMouse ? "#45a049" : "#454545")
                                radius: 3
                                Text {
                                    anchors.centerIn: parent
                                    text: "+"
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                }
                                MouseArea {
                                    id: incArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        frequencyField.text = root.normalizedFrequencyText(Math.min(100.0, root.frequency + 0.05));
                                        root.commitFrequency();
                                    }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: decArea.pressed ? "#cc4444" : (decArea.containsMouse ? "#aa3333" : "#454545")
                                radius: 3
                                Text {
                                    anchors.centerIn: parent
                                    text: "-"
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                }
                                MouseArea {
                                    id: decArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        frequencyField.text = root.normalizedFrequencyText(Math.max(0.01, root.frequency - 0.05));
                                        root.commitFrequency();
                                    }
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Rectangle {
                        width: 14
                        height: 14
                        radius: 3
                        color: root.randomFrequencyEnabled ? "#4CAF50" : "#2f2f2f"
                        border.color: root.randomFrequencyEnabled ? "#4CAF50" : "#666666"
                        Text {
                            anchors.centerIn: parent
                            text: root.randomFrequencyEnabled ? "v" : ""
                            font.pixelSize: 9
                            color: "#ffffff"
                        }
                    }
                    Text {
                        text: translationManager.currentLanguage && translationManager.textFor("random_frequency")
                        font.pixelSize: 11
                        color: "#bbbbbb"
                        Layout.fillWidth: true
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: autoClickerController.setRandomFrequencyEnabled(profileId, !root.randomFrequencyEnabled)
                    }
                }

                ColumnLayout {
                    visible: randomFrequencyEnabled
                    spacing: 2
                    Text {
                        text: translationManager.currentLanguage && translationManager.textFor("max_frequency_sec")
                        font.pixelSize: 11
                        color: "#888888"
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        color: "#3a3a3a"
                        radius: 6
                        border.color: maxFrequencyField.activeFocus ? "#5a9fd4" : "#555555"
                        border.width: 1
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 2
                            spacing: 2
                            TextField {
                                id: maxFrequencyField
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: root.normalizedFrequencyText(root.maxFrequency)
                                validator: RegularExpressionValidator {
                                    regularExpression: /^\d{0,3}[.,]?\d{0,2}$/
                                }
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                selectByMouse: true
                                background: Item {}
                                onEditingFinished: {
                                    root.commitMaxFrequency();
                                    maxFrequencyField.focus = false;
                                }
                            }
                            ColumnLayout {
                                Layout.preferredWidth: 24
                                Layout.fillHeight: true
                                spacing: 1
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: mIncArea.pressed ? "#4CAF50" : (mIncArea.containsMouse ? "#45a049" : "#454545")
                                    radius: 3
                                    Text {
                                        anchors.centerIn: parent
                                        text: "+"
                                        font.pixelSize: 11
                                        color: "#ffffff"
                                    }
                                    MouseArea {
                                        id: mIncArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            maxFrequencyField.text = root.normalizedFrequencyText(Math.min(100.0, root.maxFrequency + 0.05));
                                            root.commitMaxFrequency();
                                        }
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: mDecArea.pressed ? "#cc4444" : (mDecArea.containsMouse ? "#aa3333" : "#454545")
                                    radius: 3
                                    Text {
                                        anchors.centerIn: parent
                                        text: "-"
                                        font.pixelSize: 11
                                        color: "#ffffff"
                                    }
                                    MouseArea {
                                        id: mDecArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            maxFrequencyField.text = root.normalizedFrequencyText(Math.max(root.frequency, root.maxFrequency - 0.05));
                                            root.commitMaxFrequency();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Column 3: Mode & Control
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 2

                Text {
                    text: translationManager.currentLanguage && translationManager.textFor("mode")
                    font.pixelSize: 11
                    color: "#888888"
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    spacing: 4
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: mode === 0 ? "#4CAF50" : (tglModeArea.containsMouse ? "#3a3a3a" : "#2a2a2a")
                        radius: 6
                        Text {
                            anchors.centerIn: parent
                            text: translationManager.currentLanguage && translationManager.textFor("toggle")
                            font.pixelSize: 11
                            color: mode === 0 ? "#ffffff" : "#aaaaaa"
                        }
                        MouseArea {
                            id: tglModeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: autoClickerController.setMode(profileId, 0)
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: mode === 1 ? "#4CAF50" : (hldModeArea.containsMouse ? "#3a3a3a" : "#2a2a2a")
                        radius: 6
                        Text {
                            anchors.centerIn: parent
                            text: translationManager.currentLanguage && translationManager.textFor("hold")
                            font.pixelSize: 11
                            color: mode === 1 ? "#ffffff" : "#aaaaaa"
                        }
                        MouseArea {
                            id: hldModeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: autoClickerController.setMode(profileId, 1)
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                // Action Row
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    spacing: 6

                    // Status Text
                    ColumnLayout {
                        Layout.preferredWidth: 60
                        spacing: 1
                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            color: isActive ? "#4CAF50" : "#888888"
                            Layout.alignment: Qt.AlignHCenter
                            SequentialAnimation on color {
                                running: isActive
                                loops: Animation.Infinite
                                ColorAnimation {
                                    to: "#4CAF50"
                                    duration: 500
                                }
                                ColorAnimation {
                                    to: "#81C784"
                                    duration: 500
                                }
                            }
                        }
                        Text {
                            text: isActive ? (translationManager.currentLanguage && translationManager.textFor("active")) : (translationManager.currentLanguage && translationManager.textFor("inactive"))
                            font.pixelSize: 9
                            font.bold: true
                            color: isActive ? "#4CAF50" : "#888888"
                            Layout.alignment: Qt.AlignHCenter
                            elide: Text.ElideRight
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    // Start/Stop Button
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumWidth: 80
                        color: mode === 1 ? "#4a4a4a" : (isActive ? "#cc4444" : (tglBtnArea.containsMouse ? "#45a049" : "#4CAF50"))
                        radius: 8
                        opacity: mode === 1 ? 0.7 : 1.0
                        Text {
                            anchors.centerIn: parent
                            text: mode === 1 ? (translationManager.currentLanguage && translationManager.textFor("use_key")) : (isActive ? (translationManager.currentLanguage && translationManager.textFor("stop")) : (translationManager.currentLanguage && translationManager.textFor("start")))
                            font.pixelSize: 12
                            font.bold: true
                            color: "#ffffff"
                            elide: Text.ElideRight
                            width: parent.width - 10
                            horizontalAlignment: Text.AlignHCenter
                        }
                        MouseArea {
                            id: tglBtnArea
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: mode === 0
                            onClicked: autoClickerController.toggleProfile(profileId)
                        }
                    }

                    // Delete Button
                    Rectangle {
                        width: 38
                        height: 38
                        color: delBtnArea.containsMouse ? "#cc4444" : "#883333"
                        radius: 8
                        Text {
                            anchors.centerIn: parent
                            text: "✕"
                            font.pixelSize: 16
                            color: "#ffffff"
                        }
                        MouseArea {
                            id: delBtnArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: autoClickerController.removeProfile(profileId)
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: root
        function onFrequencyChanged() {
            if (!frequencyField.activeFocus)
                frequencyField.text = root.normalizedFrequencyText(root.frequency);
        }
        function onMaxFrequencyChanged() {
            if (!maxFrequencyField.activeFocus)
                maxFrequencyField.text = root.normalizedFrequencyText(root.maxFrequency);
        }
    }
}
